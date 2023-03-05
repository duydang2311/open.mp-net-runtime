// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <string_view>
#include "core.hpp"
#include "src/nethost.hpp"
#include "src/component.hpp"

#include "native/nethost.h"
#include "native/coreclr_delegates.h"
#include "native/hostfxr.h"
#include "src/utils/os.hpp"

#ifdef WINDOWS
#include <Windows.h>

#define STR(s) L##s
#define CH(c) L##c
#define DIR_SEPARATOR L'\\'

#else
#include <dlfcn.h>
#include <limits.h>

#define STR(s) s
#define CH(c) c
#define DIR_SEPARATOR '/'
#define MAX_PATH PATH_MAX

#endif

NetHost::NetHost(ICore* core)
{
	core_ = core;

	assert(locateExecutingAssembly());
	assert(loadHostfxr());
	assert(getNETFunctionPointer());
}

bool NetHost::locateExecutingAssembly()
{
	auto pwd = std::filesystem::current_path();
	auto gm_dir = pwd / "gamemode";
	if (!std::filesystem::exists(gm_dir) || !std::filesystem::is_directory(gm_dir))
	{
		core_->logLn(LogLevel::Error, "Missing 'gamemode' directory in server root");
		return false;
	}

	auto found = false;
	std::vector<std::filesystem::path> dirs { gm_dir };
	while (!found && !dirs.empty())
	{
		auto dir = dirs.back();
		dirs.pop_back();
		for (auto& entry : std::filesystem::directory_iterator(dir))
		{
			if (entry.is_directory())
			{
				dirs.push_back(entry.path());
				continue;
			}

			auto path = entry.path();
			if (path.filename().string().ends_with("runtimeconfig.json"))
			{
				core_->logLn(LogLevel::Message, "Found %s", std::filesystem::relative(path, pwd).c_str());
				auto dll_path = std::filesystem::path(path).replace_extension("").replace_extension("dll");
				if (!std::filesystem::exists(dll_path))
				{
					core_->logLn(LogLevel::Message, "No corresponding %s found, skipping", std::filesystem::relative(dll_path, pwd).c_str());
					continue;
				}

				found = true;
				config_path_ = path;
				dll_path_ = dll_path;
				core_->logLn(LogLevel::Message, "Found %s", std::filesystem::relative(dll_path, pwd).c_str());
				break;
			}
		}
	}
	return found;
}

NetHost* NetHost::getInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new NetHost(MainComponent::getInstance()->getCore());
	}
	return instance_;
}

#ifdef WINDOWS
void* NetHost::load_library(const char_t* path)
{
	HMODULE h = ::LoadLibraryW(path);
	assert(h != nullptr);
	return (void*)h;
}
void* NetHost::get_export(void* h, const char* name)
{
	void* f = ::GetProcAddress((HMODULE)h, name);
	assert(f != nullptr);
	return f;
}
#else
void* NetHost::loadLibrary(const char_t* path)
{
	void* h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
	assert(h != nullptr);
	return h;
}
void* NetHost::getExport(void* h, const char* name)
{
	void* f = dlsym(h, name);
	assert(f != nullptr);
	return f;
}
#endif

// Using the nethost library, discover the location of hostfxr and get exports
bool NetHost::loadHostfxr()
{
	std::filesystem::path pwd = std::filesystem::current_path();
	std::filesystem::path runtime_host_fxr_path = pwd / "runtime/host/fxr";
	if (!std::filesystem::exists(runtime_host_fxr_path))
	{
		core_->logLn(LogLevel::Error, ".NET runtime host library not found");
		return false;
	}
	auto found = false;
	std::vector<std::filesystem::path> dirs { runtime_host_fxr_path };
	while (!found && !dirs.empty())
	{
		auto dir = dirs.back();
		dirs.pop_back();
		for (auto& entry : std::filesystem::directory_iterator(dir))
		{
			if (entry.is_directory())
			{
				dirs.push_back(entry.path());
				continue;
			}

			auto path = entry.path();
#if defined(OS_WIN)
			if (path.filename() == "hostfxr.dll")
			{
				found = true;
				runtime_host_fxr_path = path;
				core_->logLn(LogLevel::Message, "Found %s", std::filesystem::relative(path, pwd).c_str());
				break;
			}
#elif defined(OS_UNIX)
			if (path.filename() == "libhostfxr.so")
			{
				found = true;
				runtime_host_fxr_path = path;
				core_->logLn(LogLevel::Message, "Found %s", std::filesystem::relative(path, pwd).c_str());
				break;
			}
#endif
		}
	}

	if (!found)
	{
		return false;
	}

	void* lib = loadLibrary(runtime_host_fxr_path.c_str());
	init_fptr_ = (hostfxr_initialize_for_runtime_config_fn)getExport(lib, "hostfxr_initialize_for_runtime_config");
	get_delegate_fptr_ = (hostfxr_get_runtime_delegate_fn)getExport(lib, "hostfxr_get_runtime_delegate");
	close_fptr_ = (hostfxr_close_fn)getExport(lib, "hostfxr_close");
	return (init_fptr_ && get_delegate_fptr_ && close_fptr_);
}

bool NetHost::getNETFunctionPointer()
{
	hostfxr_handle cxt = nullptr;
	int rc = init_fptr_(config_path_.c_str(), nullptr, &cxt);
	if (rc != 0 || cxt == nullptr)
	{
		core_->logLn(LogLevel::Error, "Failed to load .NET Core - code: %x", rc);
		close_fptr_(cxt);
		return false;
	}

	rc = get_delegate_fptr_(
		cxt,
		hdt_load_assembly_and_get_function_pointer,
		(void**)&load_assembly_and_get_function_pointer_);
	if (rc != 0 || load_assembly_and_get_function_pointer_ == nullptr)
	{
		core_->logLn(LogLevel::Error, "Failed to get load assembly function pointer - code: %x", rc);
		return false;
	}

	close_fptr_(cxt);
	return true;
}

void NetHost::invokeReadyEvent()
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * ReadyPtr)();
	ReadyPtr ready = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeCoreEvent),
		"Ready",
		(void**)&ready);
	assert(rc == 0);
	ready();
}

bool NetHost::invokePlayerRequestSpawnEvent(int playerid)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * PlayerRequestSpawnPtr)(int);
	PlayerRequestSpawnPtr playerRequestSpawn = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"PlayerRequestSpawn",
		(void**)&playerRequestSpawn);
	assert(rc == 0);
	return playerRequestSpawn(playerid);
}

bool NetHost::invokePlayerConnectEvent(int playerid)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * PlayerRequestSpawnPtr)(int);
	PlayerRequestSpawnPtr playerRequestSpawn = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"PlayerConnectEvent",
		(void**)&playerRequestSpawn);
	assert(rc == 0);
	return playerRequestSpawn(playerid);
}

void NetHost::invokeInitializeCore()
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * InitializeCorePtr)(const char*);
	auto entry_dll = std::filesystem::path(dll_path_).filename().replace_extension("").c_str();
	InitializeCorePtr initializeCore = nullptr;
	int rc = getManagedFunctionPointer(
		"Omp.Net.CoreInitializer, Omp.Net",
		"InitializeCore",
		(void**)&initializeCore);
	assert(rc == 0);
	std::cout << entry_dll << std::endl;
	initializeCore(entry_dll);
}

inline int NetHost::getManagedFunctionPointer(const char_t* typeName, const char_t* methodName, void** ptr, const char_t* delegate_type_name)
{
	return load_assembly_and_get_function_pointer_(
		dll_path_.c_str(),
		typeName,
		methodName,
		delegate_type_name,
		nullptr,
		ptr);
}

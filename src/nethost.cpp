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
#include "player.hpp"
#include "src/interop.hpp"
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

void NetHost::invokeInitializeCore()
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnIncomingConnectionPtr)(const char*);
	auto entry_dll = std::filesystem::path(dll_path_).filename().replace_extension("").c_str();
	OnIncomingConnectionPtr initializeCore = nullptr;
	int rc = getManagedFunctionPointer(
		"Omp.Net.CoreInitializer, Omp.Net",
		"InitializeCore",
		(void**)&initializeCore);
	assert(rc == 0);
	initializeCore(entry_dll);
}

void NetHost::invokeOnReady()
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnReadyPtr)();
	OnReadyPtr onReady = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeCoreEvent),
		"OnReady",
		(void**)&onReady);
	assert(rc == 0);
	onReady();
}

void NetHost::invokeOnIncomingConnection(IPlayer& player, StringView ipAddress, unsigned short port)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnIncomingConnectionPtr)(UnmanagedEntityId, const char*, unsigned short);
	OnIncomingConnectionPtr onIncomingConnection = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeCoreEvent),
		"OnIncomingConnection",
		(void**)&onIncomingConnection);
	assert(rc == 0);
	onIncomingConnection(UnmanagedEntityId(player), ipAddress.data(), port);
}

void NetHost::invokeOnPlayerConnect(IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerConnectPtr)(UnmanagedEntityId);
	OnPlayerConnectPtr onPlayerConnect = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerConnect",
		(void**)&onPlayerConnect);
	assert(rc == 0);
	onPlayerConnect(UnmanagedEntityId(player));
}

void NetHost::invokeOnPlayerDisconnect(IPlayer& player, PeerDisconnectReason reason)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerDisconnectPtr)(UnmanagedEntityId, PeerDisconnectReason);
	OnPlayerDisconnectPtr onPlayerDisconnect = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerDisconnect",
		(void**)&onPlayerDisconnect);
	assert(rc == 0);
	onPlayerDisconnect(UnmanagedEntityId(player), reason);
}

void NetHost::invokeOnPlayerClientInit(IPlayer& player)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerClientInitPtr)(UnmanagedEntityId);
	OnPlayerClientInitPtr onPlayerClientInit = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerClientInit",
		(void**)&onPlayerClientInit);
	assert(rc == 0);
	onPlayerClientInit(UnmanagedEntityId(player));
}

bool NetHost::invokeOnPlayerRequestSpawn(IPlayer& player)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerRequestSpawnPtr)(UnmanagedEntityId);
	OnPlayerRequestSpawnPtr onPlayerRequestSpawn = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerRequestSpawn",
		(void**)&onPlayerRequestSpawn);
	assert(rc == 0);
	return onPlayerRequestSpawn(UnmanagedEntityId(player));
}

void NetHost::invokeOnPlayerSpawn(IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerSpawnPtr)(UnmanagedEntityId);
	OnPlayerSpawnPtr onPlayerSpawn = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerSpawn",
		(void**)&onPlayerSpawn);
	assert(rc == 0);
	onPlayerSpawn(UnmanagedEntityId(player));
}

void NetHost::invokeOnPlayerStreamIn(IPlayer& player, IPlayer& forPlayer)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerStreamInPtr)(UnmanagedEntityId, UnmanagedEntityId);
	OnPlayerStreamInPtr onPlayerStreamIn = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerStreamIn",
		(void**)&onPlayerStreamIn);
	assert(rc == 0);
	onPlayerStreamIn(UnmanagedEntityId(player), UnmanagedEntityId(forPlayer));
}

void NetHost::invokeOnPlayerStreamOut(IPlayer& player, IPlayer& forPlayer)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerStreamOutPtr)(UnmanagedEntityId, UnmanagedEntityId);
	OnPlayerStreamOutPtr onPlayerStreamOut = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerStreamOut",
		(void**)&onPlayerStreamOut);
	assert(rc == 0);
	onPlayerStreamOut(UnmanagedEntityId(player), UnmanagedEntityId(forPlayer));
}

bool NetHost::invokeOnPlayerText(IPlayer& player, StringView message)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerTextPtr)(UnmanagedEntityId, const char*);
	OnPlayerTextPtr onPlayerText = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerText",
		(void**)&onPlayerText);
	assert(rc == 0);
	return onPlayerText(UnmanagedEntityId(player), message.data());
}

bool NetHost::invokeOnPlayerCommandText(IPlayer& player, StringView message)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerCommandTextPtr)(UnmanagedEntityId, const char*);
	OnPlayerCommandTextPtr onPlayerCommandText = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerCommandText",
		(void**)&onPlayerCommandText);
	assert(rc == 0);
	return onPlayerCommandText(UnmanagedEntityId(player), message.data());
}

bool NetHost::invokeOnPlayerShotMissed(IPlayer& player, const PlayerBulletData& bulletData)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerShotMissedPtr)(UnmanagedEntityId, PlayerBulletData);
	OnPlayerShotMissedPtr onPlayerShotMissed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerShotMissed",
		(void**)&onPlayerShotMissed);
	assert(rc == 0);
	return onPlayerShotMissed(UnmanagedEntityId(player), bulletData);
}

bool NetHost::invokeOnPlayerShotPlayer(IPlayer& player, IPlayer& target, const PlayerBulletData& bulletData)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerShotPlayerPtr)(UnmanagedEntityId, UnmanagedEntityId, PlayerBulletData);
	OnPlayerShotPlayerPtr onPlayerShotPlayer = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerShotPlayer",
		(void**)&onPlayerShotPlayer);
	assert(rc == 0);
	return onPlayerShotPlayer(UnmanagedEntityId(player), UnmanagedEntityId(target), bulletData);
}

bool NetHost::invokeOnPlayerShotVehicle(IPlayer& player, IVehicle& target, const PlayerBulletData& bulletData)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerShotVehiclePtr)(UnmanagedEntityId, UnmanagedEntityId, PlayerBulletData);
	OnPlayerShotVehiclePtr onPlayerShotVehicle = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerShotVehicle",
		(void**)&onPlayerShotVehicle);
	assert(rc == 0);
	return onPlayerShotVehicle(UnmanagedEntityId(player), UnmanagedEntityId(target), bulletData);
}

bool NetHost::invokeOnPlayerShotObject(IPlayer& player, IObject& target, const PlayerBulletData& bulletData)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerShotObjectPtr)(UnmanagedEntityId, UnmanagedEntityId, PlayerBulletData);
	OnPlayerShotObjectPtr onPlayerShotObject = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerShotObject",
		(void**)&onPlayerShotObject);
	assert(rc == 0);
	return onPlayerShotObject(UnmanagedEntityId(player), UnmanagedEntityId(target), bulletData);
}

bool NetHost::invokeOnPlayerShotPlayerObject(IPlayer& player, IPlayerObject& target, const PlayerBulletData& bulletData)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerShotPlayerObjectPtr)(UnmanagedEntityId, UnmanagedEntityId, PlayerBulletData);
	OnPlayerShotPlayerObjectPtr onPlayerShotPlayerObject = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerShotPlayerObject",
		(void**)&onPlayerShotPlayerObject);
	assert(rc == 0);
	return onPlayerShotPlayerObject(UnmanagedEntityId(player), UnmanagedEntityId(target), bulletData);
}

void NetHost::invokeOnPlayerScoreChange(IPlayer& player, int score)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerScoreChangePtr)(UnmanagedEntityId, int);
	OnPlayerScoreChangePtr onPlayerScoreChange = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerScoreChange",
		(void**)&onPlayerScoreChange);
	assert(rc == 0);
	onPlayerScoreChange(UnmanagedEntityId(player), score);
}

void NetHost::invokeOnPlayerNameChange(IPlayer& player, StringView oldName)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerNameChangePtr)(UnmanagedEntityId, const char*);
	OnPlayerNameChangePtr onPlayerNameChange = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerNameChange",
		(void**)&onPlayerNameChange);
	assert(rc == 0);
	onPlayerNameChange(UnmanagedEntityId(player), oldName.data());
}

void NetHost::invokeOnPlayerInteriorChange(IPlayer& player, unsigned newInterior, unsigned oldInterior)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerInteriorChangePtr)(UnmanagedEntityId, unsigned, unsigned);
	OnPlayerInteriorChangePtr onPlayerInteriorChange = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerInteriorChange",
		(void**)&onPlayerInteriorChange);
	assert(rc == 0);
	onPlayerInteriorChange(UnmanagedEntityId(player), newInterior, oldInterior);
}

void NetHost::invokeOnPlayerStateChange(IPlayer& player, PlayerState newState, PlayerState oldState)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerStateChangePtr)(UnmanagedEntityId, PlayerState, PlayerState);
	OnPlayerStateChangePtr onPlayerStateChange = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerStateChange",
		(void**)&onPlayerStateChange);
	assert(rc == 0);
	onPlayerStateChange(UnmanagedEntityId(player), newState, oldState);
}

void NetHost::invokeOnPlayerKeyStateChange(IPlayer& player, uint32_t newKeys, uint32_t oldKeys)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerKeyStateChangePtr)(UnmanagedEntityId, uint32_t, uint32_t);
	OnPlayerKeyStateChangePtr onPlayerKeyStateChange = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerKeyStateChange",
		(void**)&onPlayerKeyStateChange);
	assert(rc == 0);
	onPlayerKeyStateChange(UnmanagedEntityId(player), newKeys, oldKeys);
}

void NetHost::invokeOnPlayerDeath(IPlayer& player, IPlayer* killer, int reason)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerDeathPtr)(UnmanagedEntityId, UnmanagedEntityId, int);
	OnPlayerDeathPtr onPlayerDeath = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerDeath",
		(void**)&onPlayerDeath);
	assert(rc == 0);
	onPlayerDeath(UnmanagedEntityId(player), UnmanagedEntityId(killer), reason);
}

void NetHost::invokeOnPlayerTakeDamage(IPlayer& player, IPlayer* from, float amount, unsigned weapon, BodyPart part)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerTakeDamagePtr)(UnmanagedEntityId, UnmanagedEntityId, float, unsigned, BodyPart);
	OnPlayerTakeDamagePtr onPlayerTakeDamage = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerTakeDamage",
		(void**)&onPlayerTakeDamage);
	assert(rc == 0);
	onPlayerTakeDamage(UnmanagedEntityId(player), UnmanagedEntityId(from), amount, weapon, part);
}

void NetHost::invokeOnPlayerGiveDamage(IPlayer& player, IPlayer& to, float amount, unsigned weapon, BodyPart part)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerGiveDamagePtr)(UnmanagedEntityId, UnmanagedEntityId, float, unsigned, BodyPart);
	OnPlayerGiveDamagePtr onPlayerGiveDamage = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerGiveDamage",
		(void**)&onPlayerGiveDamage);
	assert(rc == 0);
	onPlayerGiveDamage(UnmanagedEntityId(player), UnmanagedEntityId(to), amount, weapon, part);
}

void NetHost::invokeOnPlayerClickMap(IPlayer& player, Vector3 pos)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerClickMapPtr)(UnmanagedEntityId, Vector3);
	OnPlayerClickMapPtr onPlayerClickMap = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerClickMap",
		(void**)&onPlayerClickMap);
	assert(rc == 0);
	onPlayerClickMap(UnmanagedEntityId(player), pos);
}

void NetHost::invokeOnPlayerClickPlayer(IPlayer& player, IPlayer& clicked, PlayerClickSource source)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerClickPlayerPtr)(UnmanagedEntityId, UnmanagedEntityId, PlayerClickSource);
	OnPlayerClickPlayerPtr onPlayerClickPlayer = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerClickPlayer",
		(void**)&onPlayerClickPlayer);
	assert(rc == 0);
	onPlayerClickPlayer(UnmanagedEntityId(player), UnmanagedEntityId(clicked), source);
}

void NetHost::invokeOnClientCheckResponse(IPlayer& player, int actionType, int address, int results)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnClientCheckResponsePtr)(UnmanagedEntityId, int, int, int);
	OnClientCheckResponsePtr onClientCheckResponse = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnClientCheckResponse",
		(void**)&onClientCheckResponse);
	assert(rc == 0);
	onClientCheckResponse(UnmanagedEntityId(player), actionType, address, results);
}

bool NetHost::invokeOnPlayerUpdate(IPlayer& player, TimePoint now)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerUpdatePtr)(UnmanagedEntityId, long long);
	OnPlayerUpdatePtr onPlayerUpdate = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerUpdate",
		(void**)&onPlayerUpdate);
	assert(rc == 0);
	return onPlayerUpdate(UnmanagedEntityId(player), now.time_since_epoch().count());
}

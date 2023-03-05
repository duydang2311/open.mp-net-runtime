#pragma once

#include <filesystem>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <string_view>

#include "core.hpp"
#include "native/nethost.h"
#include "native/coreclr_delegates.h"
#include "native/hostfxr.h"

#define STR_(X) #X
#define CAPI_TYPE_NAME(X) "Omp.Net.CApi." STR_(X) ", Omp.Net.CApi"

class NetHost
{
public:
	static NetHost* getInstance();

	void invokeInitializeCore();
	bool invokePlayerRequestSpawnEvent(int playerid);
	bool invokePlayerConnectEvent(int playerid);
	void invokeScriptStartEvent();
	void invokeTick();

private:
	inline static const char* event_type_name_ = "Omp.Net.CApi.Events.NativePlayerEvent, Omp.Net.CApi";
	inline static NetHost* instance_ = nullptr;
	ICore* core_;
	std::filesystem::path dll_path_;
	std::filesystem::path config_path_;
	std::string_view omp_capi_type_name_ = "Omp.Net.CApi";

	hostfxr_initialize_for_runtime_config_fn init_fptr_;
	hostfxr_get_runtime_delegate_fn get_delegate_fptr_;
	hostfxr_close_fn close_fptr_;
	load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer_;

	NetHost(ICore* core);
	bool locateExecutingAssembly();
	bool loadHostfxr();
	bool getNETFunctionPointer();
	void* loadLibrary(const char_t*);
	void* getExport(void*, const char*);
	int getManagedFunctionPointer(const char* typeName, const char* methodName, void** ptr, const char_t* delegate_type_name = UNMANAGEDCALLERSONLY_METHOD);
};

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

#include "src/interop.hpp"

#define STR_(X) #X
#define CAPI_TYPE_NAME(X) "Omp.Net.CApi." STR_(X) ", Omp.Net.CApi"

class NetHost
{
public:
	static NetHost* getInstance();

	void invokeInitializeCore();

	void invokeOnReady();

	void invokeOnIncomingConnection(IPlayer& player, StringView ipAddress, unsigned short port);
	void invokeOnPlayerConnect(IPlayer& player);
	void invokeOnPlayerDisconnect(IPlayer& player, PeerDisconnectReason reason);
	void invokeOnPlayerClientInit(IPlayer& player);

	bool invokeOnPlayerRequestSpawn(IPlayer& player);
	void invokeOnPlayerSpawn(IPlayer& player);

	void invokeOnPlayerStreamIn(IPlayer& player, IPlayer& forPlayer);
	void invokeOnPlayerStreamOut(IPlayer& player, IPlayer& forPlayer);
	bool invokeOnPlayerText(IPlayer& player, StringView message);
	bool invokeOnPlayerCommandText(IPlayer& player, StringView message);
	bool invokeOnPlayerShotMissed(IPlayer& player, const PlayerBulletData& bulletData);
	bool invokeOnPlayerShotPlayer(IPlayer& player, IPlayer& target, const PlayerBulletData& bulletData);
	bool invokeOnPlayerShotVehicle(IPlayer& player, IVehicle& target, const PlayerBulletData& bulletData);
	bool invokeOnPlayerShotObject(IPlayer& player, IObject& target, const PlayerBulletData& bulletData);
	bool invokeOnPlayerShotPlayerObject(IPlayer& player, IPlayerObject& target, const PlayerBulletData& bulletData);
	void invokeOnPlayerScoreChange(IPlayer& player, int score);
	void invokeOnPlayerNameChange(IPlayer& player, StringView oldName);
	void invokeOnPlayerInteriorChange(IPlayer& player, unsigned newInterior, unsigned oldInterior);
	void invokeOnPlayerStateChange(IPlayer& player, PlayerState newState, PlayerState oldState);
	void invokeOnPlayerKeyStateChange(IPlayer& player, uint32_t newKeys, uint32_t oldKeys);
	void invokeOnPlayerDeath(IPlayer& player, IPlayer* killer, int reason);
	void invokeOnPlayerTakeDamage(IPlayer& player, IPlayer* from, float amount, unsigned weapon, BodyPart part);
	void invokeOnPlayerGiveDamage(IPlayer& player, IPlayer& to, float amount, unsigned weapon, BodyPart part);
	void invokeOnPlayerClickMap(IPlayer& player, Vector3 pos);
	void invokeOnPlayerClickPlayer(IPlayer& player, IPlayer& clicked, PlayerClickSource source);
	void invokeOnClientCheckResponse(IPlayer& player, int actionType, int address, int results);
	bool invokeOnPlayerUpdate(IPlayer& player, TimePoint now);

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

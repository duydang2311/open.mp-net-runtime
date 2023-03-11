// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include "src/nethost.hpp"
#include "src/component.hpp"
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
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnIncomingConnectionPtr)(EntityId, const char*, unsigned short);
	OnIncomingConnectionPtr onIncomingConnection = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnIncomingConnection",
		(void**)&onIncomingConnection);
	assert(rc == 0);
	onIncomingConnection(EntityId(player), ipAddress.data(), port);
}

void NetHost::invokeOnPlayerConnect(IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerConnectPtr)(EntityId);
	OnPlayerConnectPtr onPlayerConnect = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerConnect",
		(void**)&onPlayerConnect);
	assert(rc == 0);
	onPlayerConnect(EntityId(player));
}

void NetHost::invokeOnPlayerDisconnect(IPlayer& player, PeerDisconnectReason reason)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerDisconnectPtr)(EntityId, PeerDisconnectReason);
	OnPlayerDisconnectPtr onPlayerDisconnect = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerDisconnect",
		(void**)&onPlayerDisconnect);
	assert(rc == 0);
	onPlayerDisconnect(EntityId(player), reason);
}

void NetHost::invokeOnPlayerClientInit(IPlayer& player)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerClientInitPtr)(EntityId);
	OnPlayerClientInitPtr onPlayerClientInit = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerClientInit",
		(void**)&onPlayerClientInit);
	assert(rc == 0);
	onPlayerClientInit(EntityId(player));
}

bool NetHost::invokeOnPlayerRequestSpawn(IPlayer& player)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerRequestSpawnPtr)(EntityId);
	OnPlayerRequestSpawnPtr onPlayerRequestSpawn = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerRequestSpawn",
		(void**)&onPlayerRequestSpawn);
	assert(rc == 0);
	return onPlayerRequestSpawn(EntityId(player));
}

void NetHost::invokeOnPlayerSpawn(IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerSpawnPtr)(EntityId);
	OnPlayerSpawnPtr onPlayerSpawn = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerSpawn",
		(void**)&onPlayerSpawn);
	assert(rc == 0);
	onPlayerSpawn(EntityId(player));
}

void NetHost::invokeOnPlayerStreamIn(IPlayer& player, IPlayer& forPlayer)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerStreamInPtr)(EntityId, EntityId);
	OnPlayerStreamInPtr onPlayerStreamIn = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerStreamIn",
		(void**)&onPlayerStreamIn);
	assert(rc == 0);
	onPlayerStreamIn(EntityId(player), EntityId(forPlayer));
}

void NetHost::invokeOnPlayerStreamOut(IPlayer& player, IPlayer& forPlayer)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerStreamOutPtr)(EntityId, EntityId);
	OnPlayerStreamOutPtr onPlayerStreamOut = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerStreamOut",
		(void**)&onPlayerStreamOut);
	assert(rc == 0);
	onPlayerStreamOut(EntityId(player), EntityId(forPlayer));
}

bool NetHost::invokeOnPlayerText(IPlayer& player, StringView message)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerTextPtr)(EntityId, const char*);
	OnPlayerTextPtr onPlayerText = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerText",
		(void**)&onPlayerText);
	assert(rc == 0);
	return onPlayerText(EntityId(player), message.data());
}

bool NetHost::invokeOnPlayerCommandText(IPlayer& player, StringView message)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerCommandTextPtr)(EntityId, const char*);
	OnPlayerCommandTextPtr onPlayerCommandText = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerCommandText",
		(void**)&onPlayerCommandText);
	assert(rc == 0);
	return onPlayerCommandText(EntityId(player), message.data());
}

bool NetHost::invokeOnPlayerShotMissed(IPlayer& player, const PlayerBulletData& bulletData)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerShotMissedPtr)(EntityId, PlayerBulletData);
	OnPlayerShotMissedPtr onPlayerShotMissed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerShotMissed",
		(void**)&onPlayerShotMissed);
	assert(rc == 0);
	return onPlayerShotMissed(EntityId(player), bulletData);
}

bool NetHost::invokeOnPlayerShotPlayer(IPlayer& player, IPlayer& target, const PlayerBulletData& bulletData)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerShotPlayerPtr)(EntityId, EntityId, PlayerBulletData);
	OnPlayerShotPlayerPtr onPlayerShotPlayer = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerShotPlayer",
		(void**)&onPlayerShotPlayer);
	assert(rc == 0);
	return onPlayerShotPlayer(EntityId(player), EntityId(target), bulletData);
}

bool NetHost::invokeOnPlayerShotVehicle(IPlayer& player, IVehicle& target, const PlayerBulletData& bulletData)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerShotVehiclePtr)(EntityId, EntityId, PlayerBulletData);
	OnPlayerShotVehiclePtr onPlayerShotVehicle = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerShotVehicle",
		(void**)&onPlayerShotVehicle);
	assert(rc == 0);
	return onPlayerShotVehicle(EntityId(player), EntityId(target), bulletData);
}

bool NetHost::invokeOnPlayerShotObject(IPlayer& player, IObject& target, const PlayerBulletData& bulletData)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerShotObjectPtr)(EntityId, EntityId, PlayerBulletData);
	OnPlayerShotObjectPtr onPlayerShotObject = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerShotObject",
		(void**)&onPlayerShotObject);
	assert(rc == 0);
	return onPlayerShotObject(EntityId(player), EntityId(target), bulletData);
}

bool NetHost::invokeOnPlayerShotPlayerObject(IPlayer& player, IPlayerObject& target, const PlayerBulletData& bulletData)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerShotPlayerObjectPtr)(EntityId, EntityId, PlayerBulletData);
	OnPlayerShotPlayerObjectPtr onPlayerShotPlayerObject = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerShotPlayerObject",
		(void**)&onPlayerShotPlayerObject);
	assert(rc == 0);
	return onPlayerShotPlayerObject(EntityId(player), EntityId(target), bulletData);
}

void NetHost::invokeOnPlayerScoreChange(IPlayer& player, int score)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerScoreChangePtr)(EntityId, int);
	OnPlayerScoreChangePtr onPlayerScoreChange = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerScoreChange",
		(void**)&onPlayerScoreChange);
	assert(rc == 0);
	onPlayerScoreChange(EntityId(player), score);
}

void NetHost::invokeOnPlayerNameChange(IPlayer& player, StringView oldName)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerNameChangePtr)(EntityId, const char*);
	OnPlayerNameChangePtr onPlayerNameChange = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerNameChange",
		(void**)&onPlayerNameChange);
	assert(rc == 0);
	onPlayerNameChange(EntityId(player), oldName.data());
}

void NetHost::invokeOnPlayerInteriorChange(IPlayer& player, unsigned newInterior, unsigned oldInterior)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerInteriorChangePtr)(EntityId, unsigned, unsigned);
	OnPlayerInteriorChangePtr onPlayerInteriorChange = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerInteriorChange",
		(void**)&onPlayerInteriorChange);
	assert(rc == 0);
	onPlayerInteriorChange(EntityId(player), newInterior, oldInterior);
}

void NetHost::invokeOnPlayerStateChange(IPlayer& player, PlayerState newState, PlayerState oldState)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerStateChangePtr)(EntityId, PlayerState, PlayerState);
	OnPlayerStateChangePtr onPlayerStateChange = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerStateChange",
		(void**)&onPlayerStateChange);
	assert(rc == 0);
	onPlayerStateChange(EntityId(player), newState, oldState);
}

void NetHost::invokeOnPlayerKeyStateChange(IPlayer& player, uint32_t newKeys, uint32_t oldKeys)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerKeyStateChangePtr)(EntityId, uint32_t, uint32_t);
	OnPlayerKeyStateChangePtr onPlayerKeyStateChange = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerKeyStateChange",
		(void**)&onPlayerKeyStateChange);
	assert(rc == 0);
	onPlayerKeyStateChange(EntityId(player), newKeys, oldKeys);
}

void NetHost::invokeOnPlayerDeath(IPlayer& player, IPlayer* killer, int reason)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerDeathPtr)(EntityId, EntityId, int);
	OnPlayerDeathPtr onPlayerDeath = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerDeath",
		(void**)&onPlayerDeath);
	assert(rc == 0);
	onPlayerDeath(EntityId(player), EntityId(killer), reason);
}

void NetHost::invokeOnPlayerTakeDamage(IPlayer& player, IPlayer* from, float amount, unsigned weapon, BodyPart part)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerTakeDamagePtr)(EntityId, EntityId, float, unsigned, BodyPart);
	OnPlayerTakeDamagePtr onPlayerTakeDamage = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerTakeDamage",
		(void**)&onPlayerTakeDamage);
	assert(rc == 0);
	onPlayerTakeDamage(EntityId(player), EntityId(from), amount, weapon, part);
}

void NetHost::invokeOnPlayerGiveDamage(IPlayer& player, IPlayer& to, float amount, unsigned weapon, BodyPart part)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerGiveDamagePtr)(EntityId, EntityId, float, unsigned, BodyPart);
	OnPlayerGiveDamagePtr onPlayerGiveDamage = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerGiveDamage",
		(void**)&onPlayerGiveDamage);
	assert(rc == 0);
	onPlayerGiveDamage(EntityId(player), EntityId(to), amount, weapon, part);
}

void NetHost::invokeOnPlayerClickMap(IPlayer& player, Vector3 pos)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerClickMapPtr)(EntityId, Vector3);
	OnPlayerClickMapPtr onPlayerClickMap = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerClickMap",
		(void**)&onPlayerClickMap);
	assert(rc == 0);
	onPlayerClickMap(EntityId(player), pos);
}

void NetHost::invokeOnPlayerClickPlayer(IPlayer& player, IPlayer& clicked, PlayerClickSource source)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerClickPlayerPtr)(EntityId, EntityId, PlayerClickSource);
	OnPlayerClickPlayerPtr onPlayerClickPlayer = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerClickPlayer",
		(void**)&onPlayerClickPlayer);
	assert(rc == 0);
	onPlayerClickPlayer(EntityId(player), EntityId(clicked), source);
}

void NetHost::invokeOnClientCheckResponse(IPlayer& player, int actionType, int address, int results)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnClientCheckResponsePtr)(EntityId, int, int, int);
	OnClientCheckResponsePtr onClientCheckResponse = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnClientCheckResponse",
		(void**)&onClientCheckResponse);
	assert(rc == 0);
	onClientCheckResponse(EntityId(player), actionType, address, results);
}

bool NetHost::invokeOnPlayerUpdate(IPlayer& player, TimePoint now)
{
	typedef bool(CORECLR_DELEGATE_CALLTYPE * OnPlayerUpdatePtr)(EntityId, long long);
	OnPlayerUpdatePtr onPlayerUpdate = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePlayerEvent),
		"OnPlayerUpdate",
		(void**)&onPlayerUpdate);
	assert(rc == 0);
	return onPlayerUpdate(EntityId(player), now.time_since_epoch().count());
}

void NetHost::invokeOnPlayerGiveDamageActor(IPlayer& player, IActor& actor, float amount, unsigned weapon, BodyPart part)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerGiveDamageActorPtr)(EntityId, EntityId, float, unsigned, BodyPart);
	OnPlayerGiveDamageActorPtr onPlayerGiveDamageActor = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeActorEvent),
		"OnPlayerGiveDamageActor",
		(void**)&onPlayerGiveDamageActor);
	assert(rc == 0);
	onPlayerGiveDamageActor(EntityId(player), EntityId(actor), amount, weapon, part);
}

void NetHost::invokeOnActorStreamOut(IActor& actor, IPlayer& forPlayer)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnActorStreamOutPtr)(EntityId, EntityId);
	OnActorStreamOutPtr onActorStreamOut = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeActorEvent),
		"OnActorStreamOut",
		(void**)&onActorStreamOut);
	assert(rc == 0);
	onActorStreamOut(EntityId(actor), EntityId(forPlayer));
}

void NetHost::invokeOnActorStreamIn(IActor& actor, IPlayer& forPlayer)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnActorStreamInPtr)(EntityId, EntityId);
	OnActorStreamInPtr onActorStreamIn = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeActorEvent),
		"OnActorStreamIn",
		(void**)&onActorStreamIn);
	assert(rc == 0);
	onActorStreamIn(EntityId(actor), EntityId(forPlayer));
}

void NetHost::invokeOnPlayerEnterCheckpoint(IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerEnterCheckpointPtr)(EntityId);
	OnPlayerEnterCheckpointPtr onPlayerEnterCheckpoint = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeCheckpointEvent),
		"OnPlayerEnterCheckpoint",
		(void**)&onPlayerEnterCheckpoint);
	assert(rc == 0);
	onPlayerEnterCheckpoint(EntityId(player));
}

void NetHost::invokeOnPlayerLeaveCheckpoint(IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerLeaveCheckpointPtr)(EntityId);
	OnPlayerLeaveCheckpointPtr onPlayerLeaveCheckpoint = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeCheckpointEvent),
		"OnPlayerLeaveCheckpoint",
		(void**)&onPlayerLeaveCheckpoint);
	assert(rc == 0);
	onPlayerLeaveCheckpoint(EntityId(player));
}

void NetHost::invokeOnPlayerEnterRaceCheckpoint(IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerEnterRaceCheckpointPtr)(EntityId);
	OnPlayerEnterRaceCheckpointPtr onPlayerEnterRaceCheckpoint = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeCheckpointEvent),
		"OnPlayerEnterRaceCheckpoint",
		(void**)&onPlayerEnterRaceCheckpoint);
	assert(rc == 0);
	onPlayerEnterRaceCheckpoint(EntityId(player));
}

void NetHost::invokeOnPlayerLeaveRaceCheckpoint(IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerLeaveRaceCheckpointPtr)(EntityId);
	OnPlayerLeaveRaceCheckpointPtr onPlayerLeaveRaceCheckpoint = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeCheckpointEvent),
		"OnPlayerLeaveRaceCheckpoint",
		(void**)&onPlayerLeaveRaceCheckpoint);
	assert(rc == 0);
	onPlayerLeaveRaceCheckpoint(EntityId(player));
}

bool NetHost::invokeOnPlayerRequestClass(IPlayer& player, unsigned int classId)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerRequestClassPtr)(EntityId, unsigned int);
	OnPlayerRequestClassPtr onPlayerRequestClass = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeClassEvent),
		"OnPlayerRequestClass",
		(void**)&onPlayerRequestClass);
	assert(rc == 0);
	return onPlayerRequestClass(EntityId(player), classId);
}

bool NetHost::invokeOnConsoleText(StringView command, StringView parameters, const ConsoleCommandSenderData& sender)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnConsoleTextPtr)(const char*, const char*, const ConsoleCommandSenderData*);
	OnConsoleTextPtr onConsoleText = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeConsoleEvent),
		"OnConsoleText",
		(void**)&onConsoleText);
	assert(rc == 0);
	return onConsoleText(command.data(), parameters.data(), &sender);
}

void NetHost::invokeOnRconLoginAttempt(IPlayer& player, StringView password, bool success)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnRconLoginAttemptPtr)(EntityId, const char*, bool);
	OnRconLoginAttemptPtr onRconLoginAttempt = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeConsoleEvent),
		"OnRconLoginAttempt",
		(void**)&onRconLoginAttempt);
	assert(rc == 0);
	onRconLoginAttempt(EntityId(player), password.data(), success);
}

void NetHost::invokeOnConsoleCommandListRequest(FlatHashSet<StringView>& commands)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnConsoleCommandListRequestPtr)(FlatHashSet<StringView>*);
	OnConsoleCommandListRequestPtr onConsoleCommandListRequest = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeConsoleEvent),
		"OnConsoleCommandListRequest",
		(void**)&onConsoleCommandListRequest);
	assert(rc == 0);
	onConsoleCommandListRequest(&commands);
}

void NetHost::invokeOnPlayerFinishedDownloading(IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerFinishedDownloadingPtr)(EntityId);
	OnPlayerFinishedDownloadingPtr onPlayerFinishedDownloading = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeCustomModelEvent),
		"OnPlayerFinishedDownloading",
		(void**)&onPlayerFinishedDownloading);
	assert(rc == 0);
	onPlayerFinishedDownloading(EntityId(player));
}

bool NetHost::invokeOnPlayerRequestDownload(IPlayer& player, ModelDownloadType type, uint32_t checksum)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerRequestDownloadPtr)(EntityId, ModelDownloadType, uint32_t);
	OnPlayerRequestDownloadPtr onPlayerRequestDownload = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeCustomModelEvent),
		"OnPlayerRequestDownload",
		(void**)&onPlayerRequestDownload);
	assert(rc == 0);
	return onPlayerRequestDownload(EntityId(player), type, checksum);
}

void NetHost::invokeOnDialogResponse(IPlayer& player, int dialogId, DialogResponse response, int listItem, StringView inputText)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnDialogResponsePtr)(EntityId, int, DialogResponse, int, const char*);
	OnDialogResponsePtr onDialogResponse = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeDialogEvent),
		"OnDialogResponse",
		(void**)&onDialogResponse);
	assert(rc == 0);
	onDialogResponse(EntityId(player), dialogId, response, listItem, inputText.data());
}

void NetHost::invokeOnPlayerEnterGangZone(IPlayer& player, IGangZone& zone)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerEnterGangZonePtr)(EntityId, int);
	OnPlayerEnterGangZonePtr onPlayerEnterGangZone = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeGangZoneEvent),
		"OnPlayerEnterGangZone",
		(void**)&onPlayerEnterGangZone);
	assert(rc == 0);
	onPlayerEnterGangZone(EntityId(player), zone.getID());
}

void NetHost::invokeOnPlayerLeaveGangZone(IPlayer& player, IGangZone& zone)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerLeaveGangZonePtr)(EntityId, int);
	OnPlayerLeaveGangZonePtr onPlayerLeaveGangZone = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeGangZoneEvent),
		"OnPlayerLeaveGangZone",
		(void**)&onPlayerLeaveGangZone);
	assert(rc == 0);
	onPlayerLeaveGangZone(EntityId(player), zone.getID());
}

void NetHost::invokeOnPlayerClickGangZone(IPlayer& player, IGangZone& zone)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerClickGangZonePtr)(EntityId, int);
	OnPlayerClickGangZonePtr onPlayerClickGangZone = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeGangZoneEvent),
		"OnPlayerClickGangZone",
		(void**)&onPlayerClickGangZone);
	assert(rc == 0);
	onPlayerClickGangZone(EntityId(player), zone.getID());
}

void NetHost::invokeOnPlayerSelectedMenuRow(IPlayer& player, MenuRow row)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerSelectedMenuRowPtr)(EntityId, MenuRow);
	OnPlayerSelectedMenuRowPtr onPlayerSelectedMenuRow = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeMenuEvent),
		"OnPlayerSelectedMenuRow",
		(void**)&onPlayerSelectedMenuRow);
	assert(rc == 0);
	onPlayerSelectedMenuRow(EntityId(player), row);
}

void NetHost::invokeOnPlayerExitedMenu(IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerExitedMenuPtr)(EntityId);
	OnPlayerExitedMenuPtr onPlayerExitedMenu = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeMenuEvent),
		"OnPlayerExitedMenu",
		(void**)&onPlayerExitedMenu);
	assert(rc == 0);
	onPlayerExitedMenu(EntityId(player));
}

void NetHost::invokeOnObjectMoved(IObject& object)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnObjectMovedPtr)(EntityId);
	OnObjectMovedPtr onObjectMoved = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeObjectEvent),
		"OnObjectMoved",
		(void**)&onObjectMoved);
	assert(rc == 0);
	onObjectMoved(EntityId(object));
}

void NetHost::invokeOnPlayerObjectMoved(IPlayer& player, IPlayerObject& object)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerObjectMovedPtr)(EntityId, EntityId);
	OnPlayerObjectMovedPtr onPlayerObjectMoved = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeObjectEvent),
		"OnPlayerObjectMoved",
		(void**)&onPlayerObjectMoved);
	assert(rc == 0);
	onPlayerObjectMoved(EntityId(player), EntityId(object));
}

void NetHost::invokeOnObjectSelected(IPlayer& player, IObject& object, int model, Vector3 position)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnObjectSelectedPtr)(EntityId, EntityId, int, Vector3);
	OnObjectSelectedPtr onObjectSelected = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeObjectEvent),
		"OnObjectSelected",
		(void**)&onObjectSelected);
	assert(rc == 0);
	onObjectSelected(EntityId(player), EntityId(object), model, position);
}

void NetHost::invokeOnPlayerObjectSelected(IPlayer& player, IPlayerObject& object, int model, Vector3 position)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerObjectSelectedPtr)(EntityId, EntityId, int, Vector3);
	OnPlayerObjectSelectedPtr onPlayerObjectSelected = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeObjectEvent),
		"OnPlayerObjectSelected",
		(void**)&onPlayerObjectSelected);
	assert(rc == 0);
	onPlayerObjectSelected(EntityId(player), EntityId(object), model, position);
}

void NetHost::invokeOnObjectEdited(IPlayer& player, IObject& object, ObjectEditResponse response, Vector3 offset, Vector3 rotation)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnObjectEditedPtr)(EntityId, EntityId, ObjectEditResponse, Vector3, Vector3);
	OnObjectEditedPtr onObjectEdited = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeObjectEvent),
		"OnObjectEdited",
		(void**)&onObjectEdited);
	assert(rc == 0);
	onObjectEdited(EntityId(player), EntityId(object), response, offset, rotation);
}

void NetHost::invokeOnPlayerObjectEdited(IPlayer& player, IPlayerObject& object, ObjectEditResponse response, Vector3 offset, Vector3 rotation)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerObjectEditedPtr)(EntityId, EntityId, ObjectEditResponse, Vector3, Vector3);
	OnPlayerObjectEditedPtr onPlayerObjectEdited = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeObjectEvent),
		"OnPlayerObjectEdited",
		(void**)&onPlayerObjectEdited);
	assert(rc == 0);
	onPlayerObjectEdited(EntityId(player), EntityId(object), response, offset, rotation);
}

void NetHost::invokeOnPlayerAttachedObjectEdited(IPlayer& player, int index, bool saved, const ObjectAttachmentSlotData& data)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerAttachedObjectEditedPtr)(EntityId, int, bool, const ObjectAttachmentSlotData*);
	OnPlayerAttachedObjectEditedPtr onPlayerAttachedObjectEdited = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeObjectEvent),
		"OnPlayerAttachedObjectEdited",
		(void**)&onPlayerAttachedObjectEdited);
	assert(rc == 0);
	onPlayerAttachedObjectEdited(EntityId(player), index, saved, &data);
}

void NetHost::invokeOnPlayerPickUpPickup(IPlayer& player, IPickup& pickup)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerPickupPickupPtr)(EntityId, EntityId);
	OnPlayerPickupPickupPtr onPlayerPickupPickup = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePickupEvent),
		"OnPlayerPickupPickup",
		(void**)&onPlayerPickupPickup);
	assert(rc == 0);
	onPlayerPickupPickup(EntityId(player), EntityId(pickup));
}

void NetHost::invokeOnPlayerClickTextDraw(IPlayer& player, ITextDraw& td)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerClickTextDrawPtr)(EntityId, int);
	OnPlayerClickTextDrawPtr onPlayerClickTextDraw = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeTextDrawEvent),
		"OnPlayerClickTextDraw",
		(void**)&onPlayerClickTextDraw);
	assert(rc == 0);
	onPlayerClickTextDraw(EntityId(player), td.getID());
}

void NetHost::invokeOnPlayerClickPlayerTextDraw(IPlayer& player, IPlayerTextDraw& td)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerClickPlayerTextDrawPtr)(EntityId, int);
	OnPlayerClickPlayerTextDrawPtr onPlayerClickPlayerTextDraw = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeTextDrawEvent),
		"OnPlayerClickPlayerTextDraw",
		(void**)&onPlayerClickPlayerTextDraw);
	assert(rc == 0);
	onPlayerClickPlayerTextDraw(EntityId(player), td.getID());
}

bool NetHost::invokeOnPlayerCancelTextDrawSelection(IPlayer& player)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerCancelTextDrawSelectionPtr)(EntityId);
	OnPlayerCancelTextDrawSelectionPtr onPlayerCancelTextDrawSelection = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeTextDrawEvent),
		"OnPlayerCancelTextDrawSelection",
		(void**)&onPlayerCancelTextDrawSelection);
	assert(rc == 0);
	return onPlayerCancelTextDrawSelection(EntityId(player));
}

bool NetHost::invokeOnPlayerCancelPlayerTextDrawSelection(IPlayer& player)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnPlayerCancelPlayerTextDrawSelectionPtr)(EntityId);
	OnPlayerCancelPlayerTextDrawSelectionPtr onPlayerCancelPlayerTextDrawSelection = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeTextDrawEvent),
		"OnPlayerCancelPlayerTextDrawSelection",
		(void**)&onPlayerCancelPlayerTextDrawSelection);
	assert(rc == 0);
	return onPlayerCancelPlayerTextDrawSelection(EntityId(player));
}

void NetHost::invokeOnVehicleStreamIn(IVehicle& vehicle, IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnVehicleStreamInPtr)(EntityId, EntityId);
	OnVehicleStreamInPtr onVehicleStreamIn = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnVehicleStreamIn",
		(void**)&onVehicleStreamIn);
	assert(rc == 0);
	onVehicleStreamIn(EntityId(vehicle), EntityId(player));
}

void NetHost::invokeOnVehicleStreamOut(IVehicle& vehicle, IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnVehicleStreamOutPtr)(EntityId, EntityId);
	OnVehicleStreamOutPtr onVehicleStreamOut = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnVehicleStreamOut",
		(void**)&onVehicleStreamOut);
	assert(rc == 0);
	onVehicleStreamOut(EntityId(vehicle), EntityId(player));
}

void NetHost::invokeOnVehicleDeath(IVehicle& vehicle, IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnVehicleDeathPtr)(EntityId, EntityId);
	OnVehicleDeathPtr onVehicleDeath = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnVehicleDeath",
		(void**)&onVehicleDeath);
	assert(rc == 0);
	onVehicleDeath(EntityId(vehicle), EntityId(player));
}

void NetHost::invokeOnPlayerEnterVehicle(IPlayer& player, IVehicle& vehicle, bool passenger)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerEnterVehiclePtr)(EntityId, EntityId, bool);
	OnPlayerEnterVehiclePtr onPlayerEnterVehicle = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnPlayerEnterVehicle",
		(void**)&onPlayerEnterVehicle);
	assert(rc == 0);
	onPlayerEnterVehicle(EntityId(vehicle), EntityId(player), passenger);
}

void NetHost::invokeOnPlayerExitVehicle(IPlayer& player, IVehicle& vehicle)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerExitVehiclePtr)(EntityId, EntityId);
	OnPlayerExitVehiclePtr onPlayerExitVehicle = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnPlayerExitVehicle",
		(void**)&onPlayerExitVehicle);
	assert(rc == 0);
	onPlayerExitVehicle(EntityId(vehicle), EntityId(player));
}

void NetHost::invokeOnVehicleDamageStatusUpdate(IVehicle& vehicle, IPlayer& player)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnVehicleDamageStatusUpdatePtr)(EntityId, EntityId);
	OnVehicleDamageStatusUpdatePtr onVehicleDamageStatusUpdate = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnVehicleDamageStatusUpdate",
		(void**)&onVehicleDamageStatusUpdate);
	assert(rc == 0);
	onVehicleDamageStatusUpdate(EntityId(vehicle), EntityId(player));
}

bool NetHost::invokeOnVehiclePaintJob(IPlayer& player, IVehicle& vehicle, int paintJob)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnVehiclePaintJobPtr)(EntityId, EntityId, int);
	OnVehiclePaintJobPtr onVehiclePaintJob = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnVehiclePaintJob",
		(void**)&onVehiclePaintJob);
	assert(rc == 0);
	return onVehiclePaintJob(EntityId(vehicle), EntityId(player), paintJob);
}

bool NetHost::invokeOnVehicleMod(IPlayer& player, IVehicle& vehicle, int component)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnVehicleModPtr)(EntityId, EntityId, int);
	OnVehicleModPtr onVehicleMod = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnVehicleMod",
		(void**)&onVehicleMod);
	assert(rc == 0);
	return onVehicleMod(EntityId(vehicle), EntityId(player), component);
}

bool NetHost::invokeOnVehicleRespray(IPlayer& player, IVehicle& vehicle, int colour1, int colour2)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnVehicleResprayPtr)(EntityId, EntityId, int, int);
	OnVehicleResprayPtr onVehicleRespray = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnVehicleRespray",
		(void**)&onVehicleRespray);
	assert(rc == 0);
	return onVehicleRespray(EntityId(vehicle), EntityId(player), colour1, colour2);
}

void NetHost::invokeOnEnterExitModShop(IPlayer& player, bool enterexit, int interiorID)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnEnterExitModShopPtr)(EntityId, bool, int);
	OnEnterExitModShopPtr onEnterExitModShop = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnEnterExitModShop",
		(void**)&onEnterExitModShop);
	assert(rc == 0);
	onEnterExitModShop(EntityId(player), enterexit, interiorID);
}

void NetHost::invokeOnVehicleSpawn(IVehicle& vehicle)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnVehicleSpawnPtr)(EntityId);
	OnVehicleSpawnPtr onVehicleSpawn = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnVehicleSpawn",
		(void**)&onVehicleSpawn);
	assert(rc == 0);
	return onVehicleSpawn(EntityId(vehicle));
}

bool NetHost::invokeOnUnoccupiedVehicleUpdate(IVehicle& vehicle, IPlayer& player, UnoccupiedVehicleUpdate const updateData)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnUnoccupiedVehicleUpdatePtr)(EntityId, EntityId, UnoccupiedVehicleUpdate);
	OnUnoccupiedVehicleUpdatePtr onUnoccupiedVehicleUpdate = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnUnoccupiedVehicleUpdate",
		(void**)&onUnoccupiedVehicleUpdate);
	assert(rc == 0);
	return onUnoccupiedVehicleUpdate(EntityId(vehicle), EntityId(player), updateData);
}

bool NetHost::invokeOnTrailerUpdate(IPlayer& player, IVehicle& trailer)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnTrailerUpdatePtr)(EntityId, EntityId);
	OnTrailerUpdatePtr onTrailerUpdate = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnTrailerUpdate",
		(void**)&onTrailerUpdate);
	assert(rc == 0);
	return onTrailerUpdate(EntityId(player), EntityId(trailer));
}

bool NetHost::invokeOnVehicleSirenStateChange(IPlayer& player, IVehicle& vehicle, uint8_t sirenState)
{
	typedef int(CORECLR_DELEGATE_CALLTYPE * OnVehicleSirenStateChangePtr)(EntityId, EntityId, uint8_t);
	OnVehicleSirenStateChangePtr onVehicleSirenStateChange = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativeVehicleEvent),
		"OnVehicleSirenStateChange",
		(void**)&onVehicleSirenStateChange);
	assert(rc == 0);
	return onVehicleSirenStateChange(EntityId(player), EntityId(vehicle), sirenState);
}

void NetHost::invokeOnPoolEntryCreated(IActor& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnActorCreatedPtr)(EntityId);
	OnActorCreatedPtr onActorCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnActorCreated",
		(void**)&onActorCreated);
	assert(rc == 0);
	return onActorCreated(EntityId(entry));
}

void NetHost::invokeOnPoolEntryDestroyed(IActor& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnActorDestroyedPtr)(EntityId);
	OnActorDestroyedPtr onActorDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnActorDestroyed",
		(void**)&onActorDestroyed);
	assert(rc == 0);
	return onActorDestroyed(EntityId(entry));
}

void NetHost::invokeOnPoolEntryCreated(IPickup& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPickupCreatedPtr)(EntityId);
	OnPickupCreatedPtr onPickupCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnPickupCreated",
		(void**)&onPickupCreated);
	assert(rc == 0);
	return onPickupCreated(EntityId(entry));
}

void NetHost::invokeOnPoolEntryDestroyed(IPickup& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPickupDestroyedPtr)(EntityId);
	OnPickupDestroyedPtr onPickupDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnPickupDestroyed",
		(void**)&onPickupDestroyed);
	assert(rc == 0);
	return onPickupDestroyed(EntityId(entry));
}

void NetHost::invokeOnPoolEntryCreated(IPlayer& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerCreatedPtr)(EntityId);
	OnPlayerCreatedPtr onPlayerCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnPlayerCreated",
		(void**)&onPlayerCreated);
	assert(rc == 0);
	return onPlayerCreated(EntityId(entry));
}

void NetHost::invokeOnPoolEntryDestroyed(IPlayer& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerDestroyedPtr)(EntityId);
	OnPlayerDestroyedPtr onPlayerDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnPlayerDestroyed",
		(void**)&onPlayerDestroyed);
	assert(rc == 0);
	return onPlayerDestroyed(EntityId(entry));
}

void NetHost::invokeOnPoolEntryCreated(IVehicle& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnVehicleCreatedPtr)(EntityId);
	OnVehicleCreatedPtr onVehicleCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnVehicleCreated",
		(void**)&onVehicleCreated);
	assert(rc == 0);
	return onVehicleCreated(EntityId(entry));
}

void NetHost::invokeOnPoolEntryDestroyed(IVehicle& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnVehicleDestroyedPtr)(EntityId);
	OnVehicleDestroyedPtr onVehicleDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnVehicleDestroyed",
		(void**)&onVehicleDestroyed);
	assert(rc == 0);
	return onVehicleDestroyed(EntityId(entry));
}

void NetHost::invokeOnPoolEntryCreated(IObject& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnObjectCreatedPtr)(EntityId);
	OnObjectCreatedPtr onObjectCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnObjectCreated",
		(void**)&onObjectCreated);
	assert(rc == 0);
	return onObjectCreated(EntityId(entry));
}

void NetHost::invokeOnPoolEntryDestroyed(IObject& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnObjectDestroyedPtr)(EntityId);
	OnObjectDestroyedPtr onObjectDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnObjectDestroyed",
		(void**)&onObjectDestroyed);
	assert(rc == 0);
	return onObjectDestroyed(EntityId(entry));
}

void NetHost::invokeOnPoolEntryCreated(IPlayerObject& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerObjectCreatedPtr)(EntityId);
	OnPlayerObjectCreatedPtr onPlayerObjectCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnPlayerObjectCreated",
		(void**)&onPlayerObjectCreated);
	assert(rc == 0);
	return onPlayerObjectCreated(EntityId(entry));
}

void NetHost::invokeOnPoolEntryDestroyed(IPlayerObject& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerObjectDestroyedPtr)(EntityId);
	OnPlayerObjectDestroyedPtr onPlayerObjectDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnPlayerObjectDestroyed",
		(void**)&onPlayerObjectDestroyed);
	assert(rc == 0);
	return onPlayerObjectDestroyed(EntityId(entry));
}

void NetHost::invokeOnPoolEntryCreated(ITextLabel& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnTextLabelCreatedPtr)(EntityId);
	OnTextLabelCreatedPtr onTextLabelCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnTextLabelCreated",
		(void**)&onTextLabelCreated);
	assert(rc == 0);
	return onTextLabelCreated(EntityId(entry));
}

void NetHost::invokeOnPoolEntryDestroyed(ITextLabel& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnTextLabelDestroyedPtr)(EntityId);
	OnTextLabelDestroyedPtr onTextLabelDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnTextLabelDestroyed",
		(void**)&onTextLabelDestroyed);
	assert(rc == 0);
	return onTextLabelDestroyed(EntityId(entry));
}

void NetHost::invokeOnPoolEntryCreated(IPlayerTextLabel& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerTextLabelCreatedPtr)(EntityId);
	OnPlayerTextLabelCreatedPtr onPlayerTextLabelCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnPlayerTextLabelCreated",
		(void**)&onPlayerTextLabelCreated);
	assert(rc == 0);
	return onPlayerTextLabelCreated(EntityId(entry));
}

void NetHost::invokeOnPoolEntryDestroyed(IPlayerTextLabel& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnPlayerTextLabelDestroyedPtr)(EntityId);
	OnPlayerTextLabelDestroyedPtr onPlayerTextLabelDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnPlayerTextLabelDestroyed",
		(void**)&onPlayerTextLabelDestroyed);
	assert(rc == 0);
	return onPlayerTextLabelDestroyed(EntityId(entry));
}

void NetHost::invokeOnPoolEntryCreated(IClass& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnClassCreatedPtr)(int);
	OnClassCreatedPtr onClassCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnClassCreated",
		(void**)&onClassCreated);
	assert(rc == 0);
	return onClassCreated(entry.getID());
}

void NetHost::invokeOnPoolEntryDestroyed(IClass& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnClassDestroyedPtr)(int);
	OnClassDestroyedPtr onClassDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnClassDestroyed",
		(void**)&onClassDestroyed);
	assert(rc == 0);
	return onClassDestroyed(entry.getID());
}

void NetHost::invokeOnPoolEntryCreated(IGangZone& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnGangZoneCreatedPtr)(int);
	OnGangZoneCreatedPtr onGangZoneCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnGangZoneCreated",
		(void**)&onGangZoneCreated);
	assert(rc == 0);
	return onGangZoneCreated(entry.getID());
}

void NetHost::invokeOnPoolEntryDestroyed(IGangZone& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnGangZoneDestroyedPtr)(int);
	OnGangZoneDestroyedPtr onGangZoneDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnGangZoneDestroyed",
		(void**)&onGangZoneDestroyed);
	assert(rc == 0);
	return onGangZoneDestroyed(entry.getID());
}

void NetHost::invokeOnPoolEntryCreated(IMenu& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnMenuCreatedPtr)(int);
	OnMenuCreatedPtr onMenuCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnMenuCreated",
		(void**)&onMenuCreated);
	assert(rc == 0);
	return onMenuCreated(entry.getID());
}
void NetHost::invokeOnPoolEntryDestroyed(IMenu& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnMenuDestroyedPtr)(int);
	OnMenuDestroyedPtr onMenuDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnMenuDestroyed",
		(void**)&onMenuDestroyed);
	assert(rc == 0);
	return onMenuDestroyed(entry.getID());
}
void NetHost::invokeOnPoolEntryCreated(ITextDraw& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnTextDrawCreatedPtr)(int);
	OnTextDrawCreatedPtr onTextDrawCreated = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnTextDrawCreated",
		(void**)&onTextDrawCreated);
	assert(rc == 0);
	return onTextDrawCreated(entry.getID());
}
void NetHost::invokeOnPoolEntryDestroyed(ITextDraw& entry)
{
	typedef void(CORECLR_DELEGATE_CALLTYPE * OnTextDrawDestroyedPtr)(int);
	OnTextDrawDestroyedPtr onTextDrawDestroyed = nullptr;
	int rc = getManagedFunctionPointer(
		CAPI_TYPE_NAME(Events.NativePoolEvent),
		"OnTextDrawDestroyed",
		(void**)&onTextDrawDestroyed);
	assert(rc == 0);
	return onTextDrawDestroyed(entry.getID());
}

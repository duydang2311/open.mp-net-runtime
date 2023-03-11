#pragma once

#include <filesystem>
#include <string_view>

#include "core.hpp"
#include "Server/Components/Actors/actors.hpp"
#include "Server/Components/Checkpoints/checkpoints.hpp"
#include "Server/Components/Classes/classes.hpp"
#include "Server/Components/Console/console.hpp"
#include "Server/Components/CustomModels/custommodels.hpp"
#include "Server/Components/Dialogs/dialogs.hpp"
#include "Server/Components/GangZones/gangzones.hpp"
#include "Server/Components/Menus/menus.hpp"
#include "Server/Components/Objects/objects.hpp"
#include "Server/Components/Pickups/pickups.hpp"
#include "Server/Components/TextDraws/textdraws.hpp"
#include "Server/Components/TextLabels/textlabels.hpp"
#include "Server/Components/Vehicles/vehicles.hpp"
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

	// ActorEventHandler
	void invokeOnPlayerGiveDamageActor(IPlayer& player, IActor& actor, float amount, unsigned weapon, BodyPart part);
	void invokeOnActorStreamOut(IActor& actor, IPlayer& forPlayer);
	void invokeOnActorStreamIn(IActor& actor, IPlayer& forPlayer);

	// PlayerCheckpointEventHandler
	void invokeOnPlayerEnterCheckpoint(IPlayer& player);
	void invokeOnPlayerLeaveCheckpoint(IPlayer& player);
	void invokeOnPlayerEnterRaceCheckpoint(IPlayer& player);
	void invokeOnPlayerLeaveRaceCheckpoint(IPlayer& player);

	// ClassEventHandler
	bool invokeOnPlayerRequestClass(IPlayer& player, unsigned int classId);

	// ConsoleEventHandler
	bool invokeOnConsoleText(StringView command, StringView parameters, const ConsoleCommandSenderData& sender);
	void invokeOnRconLoginAttempt(IPlayer& player, StringView password, bool success);
	void invokeOnConsoleCommandListRequest(FlatHashSet<StringView>& commands);

	// PlayerModelsEventHandler
	void invokeOnPlayerFinishedDownloading(IPlayer& player);
	bool invokeOnPlayerRequestDownload(IPlayer& player, ModelDownloadType type, uint32_t checksum);

	// PlayerDialogEventHandler
	void invokeOnDialogResponse(IPlayer& player, int dialogId, DialogResponse response, int listItem, StringView inputText);

	// GangZoneEventHandler
	void invokeOnPlayerEnterGangZone(IPlayer& player, IGangZone& zone);
	void invokeOnPlayerLeaveGangZone(IPlayer& player, IGangZone& zone);
	void invokeOnPlayerClickGangZone(IPlayer& player, IGangZone& zone);

	// MenuEventHandler
	void invokeOnPlayerSelectedMenuRow(IPlayer& player, MenuRow row);
	void invokeOnPlayerExitedMenu(IPlayer& player);

	// ObjectEventHandler
	void invokeOnObjectMoved(IObject& object);
	void invokeOnPlayerObjectMoved(IPlayer& player, IPlayerObject& object);
	void invokeOnObjectSelected(IPlayer& player, IObject& object, int model, Vector3 position);
	void invokeOnPlayerObjectSelected(IPlayer& player, IPlayerObject& object, int model, Vector3 position);
	void invokeOnObjectEdited(IPlayer& player, IObject& object, ObjectEditResponse response, Vector3 offset, Vector3 rotation);
	void invokeOnPlayerObjectEdited(IPlayer& player, IPlayerObject& object, ObjectEditResponse response, Vector3 offset, Vector3 rotation);
	void invokeOnPlayerAttachedObjectEdited(IPlayer& player, int index, bool saved, const ObjectAttachmentSlotData& data);

	// PickupEventHandler
	void invokeOnPlayerPickUpPickup(IPlayer& player, IPickup& pickup);

	// TextDrawsEventHandler
	void invokeOnPlayerClickTextDraw(IPlayer& player, ITextDraw& td);
	void invokeOnPlayerClickPlayerTextDraw(IPlayer& player, IPlayerTextDraw& td);
	bool invokeOnPlayerCancelTextDrawSelection(IPlayer& player);
	bool invokeOnPlayerCancelPlayerTextDrawSelection(IPlayer& player);

	// VehicleEventHandler
	void invokeOnVehicleStreamIn(IVehicle& vehicle, IPlayer& player);
	void invokeOnVehicleStreamOut(IVehicle& vehicle, IPlayer& player);
	void invokeOnVehicleDeath(IVehicle& vehicle, IPlayer& player);
	void invokeOnPlayerEnterVehicle(IPlayer& player, IVehicle& vehicle, bool passenger);
	void invokeOnPlayerExitVehicle(IPlayer& player, IVehicle& vehicle);
	void invokeOnVehicleDamageStatusUpdate(IVehicle& vehicle, IPlayer& player);
	bool invokeOnVehiclePaintJob(IPlayer& player, IVehicle& vehicle, int paintJob);
	bool invokeOnVehicleMod(IPlayer& player, IVehicle& vehicle, int component);
	bool invokeOnVehicleRespray(IPlayer& player, IVehicle& vehicle, int colour1, int colour2);
	void invokeOnEnterExitModShop(IPlayer& player, bool enterexit, int interiorID);
	void invokeOnVehicleSpawn(IVehicle& vehicle);
	bool invokeOnUnoccupiedVehicleUpdate(IVehicle& vehicle, IPlayer& player, UnoccupiedVehicleUpdate const updateData);
	bool invokeOnTrailerUpdate(IPlayer& player, IVehicle& trailer);
	bool invokeOnVehicleSirenStateChange(IPlayer& player, IVehicle& vehicle, uint8_t sirenState);

	// PoolEventHandler
	void invokeOnPoolEntryCreated(IActor& entry);
	void invokeOnPoolEntryDestroyed(IActor& entry);
	void invokeOnPoolEntryCreated(IPickup& entry);
	void invokeOnPoolEntryDestroyed(IPickup& entry);
	void invokeOnPoolEntryCreated(IPlayer& entry);
	void invokeOnPoolEntryDestroyed(IPlayer& entry);
	void invokeOnPoolEntryCreated(IVehicle& entry);
	void invokeOnPoolEntryDestroyed(IVehicle& entry);
	void invokeOnPoolEntryCreated(IObject& entry);
	void invokeOnPoolEntryDestroyed(IObject& entry);
	void invokeOnPoolEntryCreated(IPlayerObject& entry);
	void invokeOnPoolEntryDestroyed(IPlayerObject& entry);
	void invokeOnPoolEntryCreated(ITextLabel& entry);
	void invokeOnPoolEntryDestroyed(ITextLabel& entry);
	void invokeOnPoolEntryCreated(IPlayerTextLabel& entry);
	void invokeOnPoolEntryDestroyed(IPlayerTextLabel& entry);
	void invokeOnPoolEntryCreated(IClass& entry);
	void invokeOnPoolEntryDestroyed(IClass& entry);
	void invokeOnPoolEntryCreated(IGangZone& entry);
	void invokeOnPoolEntryDestroyed(IGangZone& entry);
	void invokeOnPoolEntryCreated(IMenu& entry);
	void invokeOnPoolEntryDestroyed(IMenu& entry);
	void invokeOnPoolEntryCreated(ITextDraw& entry);
	void invokeOnPoolEntryDestroyed(ITextDraw& entry);

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

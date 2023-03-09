/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2022, open.mp team and contributors.
 */

#pragma once
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
#include "core.hpp"

#include "player.hpp"
#include "src/c-api/player.hpp"
#include "src/nethost.hpp"
#include "values.hpp"

using TickDelegate = void* (*)();
class MainComponent final : public IComponent,
							public CoreEventHandler,
							public PlayerSpawnEventHandler,
							public PlayerConnectEventHandler,
							public PlayerStreamEventHandler,
							public PlayerTextEventHandler,
							public PlayerShotEventHandler,
							public PlayerChangeEventHandler,
							public PlayerDamageEventHandler,
							public PlayerClickEventHandler,
							public PlayerCheckEventHandler,
							public PlayerUpdateEventHandler,
							public ActorEventHandler,
							public PlayerCheckpointEventHandler,
							public ClassEventHandler,
							public ConsoleEventHandler,
							public PlayerModelsEventHandler,
							public PlayerDialogEventHandler,
							public GangZoneEventHandler,
							public MenuEventHandler,
							public ObjectEventHandler,
							public PickupEventHandler,
							public TextDrawEventHandler,
							public VehicleEventHandler,
							public PoolEventHandler<IActor>,
							public PoolEventHandler<IPlayer>,
							public PoolEventHandler<IVehicle>,
							public PoolEventHandler<IObject>,
							public PoolEventHandler<IPlayerObject>,
							public PoolEventHandler<ITextLabel>,
							public PoolEventHandler<IPlayerTextLabel>,
							public PoolEventHandler<IPickup>
{
private:
	inline static MainComponent* instance_ = nullptr;

	ICore* core_ = nullptr;
	ITextDrawsComponent* text_draw_component_ = nullptr;
	IVehiclesComponent* vehicles_component_ = nullptr;
	IObjectsComponent* objects_component_ = nullptr;
	NetHost* host_ = nullptr;
	TickDelegate tick_delegate_ = nullptr;

public:
	PROVIDE_UID(0xEC3F51ACBB78719D);

	~MainComponent();

	StringView componentName() const override
	{
		return "open.mp-net";
	}

	SemanticVersion componentVersion() const override
	{
		return SemanticVersion(0, 0, 1, 0);
	}

	static MainComponent* getInstance();
	ICore* getCore();
	ITextDrawsComponent* getTextDrawComponent();
	IVehiclesComponent* getVehiclesComponent();
	IObjectsComponent* getObjectsComponent();
	void setTickDelegate(TickDelegate ptr);
	void free() override;

	void reset() override;
	void onLoad(ICore* c) override;
	void onInit(IComponentList* components) override;
	void onTick(Microseconds elapsed, TimePoint now) override;
	void onFree(IComponent* component) override;
	void onReady() override;

	// PlayerSpawnEventHandler
	void onIncomingConnection(IPlayer& player, StringView ipAddress, unsigned short port) override;
	void onPlayerConnect(IPlayer& player) override;
	void onPlayerDisconnect(IPlayer& player, PeerDisconnectReason reason) override;
	void onPlayerClientInit(IPlayer& player) override;

	// PlayerConnectEventHandler
	bool onPlayerRequestSpawn(IPlayer& player) override;
	void onPlayerSpawn(IPlayer& player) override;

	// PlayerStreamEventHandler
	void onPlayerStreamIn(IPlayer& player, IPlayer& forPlayer) override;
	void onPlayerStreamOut(IPlayer& player, IPlayer& forPlayer) override;

	// PlayerTextEventHandler
	bool onPlayerText(IPlayer& player, StringView message) override;
	bool onPlayerCommandText(IPlayer& player, StringView message) override;

	// PlayerShotEventHandler
	bool onPlayerShotMissed(IPlayer& player, const PlayerBulletData& bulletData) override;
	bool onPlayerShotPlayer(IPlayer& player, IPlayer& target, const PlayerBulletData& bulletData) override;
	bool onPlayerShotVehicle(IPlayer& player, IVehicle& target, const PlayerBulletData& bulletData) override;
	bool onPlayerShotObject(IPlayer& player, IObject& target, const PlayerBulletData& bulletData) override;
	bool onPlayerShotPlayerObject(IPlayer& player, IPlayerObject& target, const PlayerBulletData& bulletData) override;

	// PlayerChangeEventHandler
	void onPlayerScoreChange(IPlayer& player, int score) override;
	void onPlayerNameChange(IPlayer& player, StringView oldName) override;
	void onPlayerInteriorChange(IPlayer& player, unsigned newInterior, unsigned oldInterior) override;
	void onPlayerStateChange(IPlayer& player, PlayerState newState, PlayerState oldState) override;
	void onPlayerKeyStateChange(IPlayer& player, uint32_t newKeys, uint32_t oldKeys) override;

	// PlayerDamageEventHandler
	void onPlayerDeath(IPlayer& player, IPlayer* killer, int reason) override;
	void onPlayerTakeDamage(IPlayer& player, IPlayer* from, float amount, unsigned weapon, BodyPart part) override;
	void onPlayerGiveDamage(IPlayer& player, IPlayer& to, float amount, unsigned weapon, BodyPart part) override;

	// PlayerClickEventHandler
	void onPlayerClickMap(IPlayer& player, Vector3 pos) override;
	void onPlayerClickPlayer(IPlayer& player, IPlayer& clicked, PlayerClickSource source) override;

	// PlayerCheckEventHandler
	void onClientCheckResponse(IPlayer& player, int actionType, int address, int results) override;

	// PlayerUpdateEventHandler
	bool onPlayerUpdate(IPlayer& player, TimePoint now) override;

	// ActorEventHandler
	void onPlayerGiveDamageActor(IPlayer& player, IActor& actor, float amount, unsigned weapon, BodyPart part) override;
	void onActorStreamOut(IActor& actor, IPlayer& forPlayer) override;
	void onActorStreamIn(IActor& actor, IPlayer& forPlayer) override;

	// PlayerCheckpointEventHandler
	void onPlayerEnterCheckpoint(IPlayer& player) override;
	void onPlayerLeaveCheckpoint(IPlayer& player) override;
	void onPlayerEnterRaceCheckpoint(IPlayer& player) override;
	void onPlayerLeaveRaceCheckpoint(IPlayer& player) override;

	// ClassEventHandler
	bool onPlayerRequestClass(IPlayer& player, unsigned int classId) override;

	// ConsoleEventHandler
	bool onConsoleText(StringView command, StringView parameters, const ConsoleCommandSenderData& sender) override;
	void onRconLoginAttempt(IPlayer& player, StringView password, bool success) override;
	void onConsoleCommandListRequest(FlatHashSet<StringView>& commands) override;

	// PlayerModelsEventHandler
	void onPlayerFinishedDownloading(IPlayer& player) override;
	bool onPlayerRequestDownload(IPlayer& player, ModelDownloadType type, uint32_t checksum) override;

	// PlayerDialogEventHandler
	void onDialogResponse(IPlayer& player, int dialogId, DialogResponse response, int listItem, StringView inputText) override;

	// GangZoneEventHandler
	void onPlayerEnterGangZone(IPlayer& player, IGangZone& zone) override;
	void onPlayerLeaveGangZone(IPlayer& player, IGangZone& zone) override;
	void onPlayerClickGangZone(IPlayer& player, IGangZone& zone) override;

	// MenuEventHandler
	void onPlayerSelectedMenuRow(IPlayer& player, MenuRow row) override;
	void onPlayerExitedMenu(IPlayer& player) override;

	// ObjectEventHandler
	void onMoved(IObject& object) override;
	void onPlayerObjectMoved(IPlayer& player, IPlayerObject& object) override;
	void onObjectSelected(IPlayer& player, IObject& object, int model, Vector3 position) override;
	void onPlayerObjectSelected(IPlayer& player, IPlayerObject& object, int model, Vector3 position) override;
	void onObjectEdited(IPlayer& player, IObject& object, ObjectEditResponse response, Vector3 offset, Vector3 rotation) override;
	void onPlayerObjectEdited(IPlayer& player, IPlayerObject& object, ObjectEditResponse response, Vector3 offset, Vector3 rotation) override;
	void onPlayerAttachedObjectEdited(IPlayer& player, int index, bool saved, const ObjectAttachmentSlotData& data) override;

	// PickupEventHandler
	void onPlayerPickUpPickup(IPlayer& player, IPickup& pickup) override;

	// TextDrawsEventHandler
	void onPlayerClickTextDraw(IPlayer& player, ITextDraw& td) override;
	void onPlayerClickPlayerTextDraw(IPlayer& player, IPlayerTextDraw& td) override;
	bool onPlayerCancelTextDrawSelection(IPlayer& player) override;
	bool onPlayerCancelPlayerTextDrawSelection(IPlayer& player) override;

	// VehicleEventHandler
	void onVehicleStreamIn(IVehicle& vehicle, IPlayer& player) override;
	void onVehicleStreamOut(IVehicle& vehicle, IPlayer& player) override;
	void onVehicleDeath(IVehicle& vehicle, IPlayer& player) override;
	void onPlayerEnterVehicle(IPlayer& player, IVehicle& vehicle, bool passenger) override;
	void onPlayerExitVehicle(IPlayer& player, IVehicle& vehicle) override;
	void onVehicleDamageStatusUpdate(IVehicle& vehicle, IPlayer& player) override;
	bool onVehiclePaintJob(IPlayer& player, IVehicle& vehicle, int paintJob) override;
	bool onVehicleMod(IPlayer& player, IVehicle& vehicle, int component) override;
	bool onVehicleRespray(IPlayer& player, IVehicle& vehicle, int colour1, int colour2) override;
	void onEnterExitModShop(IPlayer& player, bool enterexit, int interiorID) override;
	void onVehicleSpawn(IVehicle& vehicle) override;
	bool onUnoccupiedVehicleUpdate(IVehicle& vehicle, IPlayer& player, UnoccupiedVehicleUpdate const updateData) override;
	bool onTrailerUpdate(IPlayer& player, IVehicle& trailer) override;
	bool onVehicleSirenStateChange(IPlayer& player, IVehicle& vehicle, uint8_t sirenState) override;

	// PoolEventHandler
	void onPoolEntryCreated(IActor& entry) override;
	void onPoolEntryDestroyed(IActor& entry) override;
	void onPoolEntryCreated(IPickup& entry) override;
	void onPoolEntryDestroyed(IPickup& entry) override;
	void onPoolEntryCreated(IPlayer& entry) override;
	void onPoolEntryDestroyed(IPlayer& entry) override;
	void onPoolEntryCreated(IVehicle& entry) override;
	void onPoolEntryDestroyed(IVehicle& entry) override;
	void onPoolEntryCreated(IObject& entry) override;
	void onPoolEntryDestroyed(IObject& entry) override;
	void onPoolEntryCreated(IPlayerObject& entry) override;
	void onPoolEntryDestroyed(IPlayerObject& entry) override;
	void onPoolEntryCreated(ITextLabel& entry) override;
	void onPoolEntryDestroyed(ITextLabel& entry) override;
	void onPoolEntryCreated(IPlayerTextLabel& entry) override;
	void onPoolEntryDestroyed(IPlayerTextLabel& entry) override;
};

#include "Server/Components/Objects/objects.hpp"
#include "src/c-api/player.hpp"
#include "types.hpp"

void Player_Kick(IEntity* player)
{
	static_cast<IPlayer*>(player)->kick();
}

void Player_Ban(IEntity* player, const char* reason)
{
	static_cast<IPlayer*>(player)->ban(reason);
}

bool Player_IsBot(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->isBot();
}

const PeerNetworkData Player_GetNetworkData(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getNetworkData();
}

unsigned Player_GetPing(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getPing();
}

bool Player_SendPacket(IEntity* player, Span<uint8_t> data, int channel, bool dispatchEvents)
{
	return static_cast<IPlayer*>(player)->sendPacket(data, channel, dispatchEvents);
}

bool Player_SendRPC(IEntity* player, int id, Span<uint8_t> data, int channel, bool dispatchEvents)
{
	return static_cast<IPlayer*>(player)->sendRPC(id, data, channel, dispatchEvents);
}

void Player_BroadcastRPCToStreamed(IEntity* player, int id, Span<uint8_t> data, int channel, bool skipFrom)
{
	static_cast<IPlayer*>(player)->broadcastRPCToStreamed(id, data, channel, skipFrom);
}

void Player_BroadcastPacketToStreamed(IEntity* player, Span<uint8_t> data, int channel, bool skipFrom)
{
	static_cast<IPlayer*>(player)->broadcastPacketToStreamed(data, channel, skipFrom);
}

void Player_BroadcastSyncPacket(IEntity* player, Span<uint8_t> data, int channel)
{
	static_cast<IPlayer*>(player)->broadcastSyncPacket(data, channel);
}

void Player_Spawn(IEntity* player)
{
	static_cast<IPlayer*>(player)->spawn();
}

ClientVersion Player_GetClientVersion(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getClientVersion();
}

const char* Player_GetClientVersionName(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getClientVersionName().data();
}

void Player_SetPositionFindZ(IEntity* player, Vector3 pos)
{
	static_cast<IPlayer*>(player)->setPositionFindZ(pos);
}

void Player_SetCameraPosition(IEntity* player, Vector3 pos)
{
	static_cast<IPlayer*>(player)->setCameraPosition(pos);
}

Vector3 Player_GetCameraPosition(IEntity* player)
{
	return static_cast<IPlayer*>(player)->getCameraPosition();
}

void Player_SetCameraLookAt(IEntity* player, Vector3 pos, PlayerCameraCutType cutType)
{
	static_cast<IPlayer*>(player)->setCameraLookAt(pos, cutType);
}

Vector3 Player_GetCameraLookAt(IEntity* player)
{
	return static_cast<IPlayer*>(player)->getCameraLookAt();
}

void Player_SetCameraBehind(IEntity* player)
{
	static_cast<IPlayer*>(player)->setCameraBehind();
}

void Player_InterpolateCameraPosition(IEntity* player, Vector3 from, Vector3 to, int time, PlayerCameraCutType cutType)
{
	static_cast<IPlayer*>(player)->interpolateCameraPosition(from, to, time, cutType);
}

void Player_InterpolateCameraLookAt(IEntity* player, Vector3 from, Vector3 to, int time, PlayerCameraCutType cutType)
{
	static_cast<IPlayer*>(player)->interpolateCameraLookAt(from, to, time, cutType);
}

void Player_AttachCameraToObject(IEntity* player, IEntity* object)
{
	static_cast<IPlayer*>(player)->attachCameraToObject(*static_cast<IObject*>(object));
}

void Player_AttachCameraToPlayerObject(IEntity* player, IEntity* playerObject)
{
	static_cast<IPlayer*>(player)->attachCameraToObject(*static_cast<IPlayerObject*>(playerObject));
}

EPlayerNameStatus Player_SetName(IEntity* player, const char* name)
{
	return static_cast<IPlayer*>(player)->setName(name);
}

const char* Player_GetName(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getName().data();
}

const char* Player_GetSerial(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getSerial().data();
}

void Player_GiveWeapon(IEntity* player, WeaponSlotData weapon)
{
	static_cast<IPlayer*>(player)->giveWeapon(weapon);
}

void Player_RemoveWeapon(IEntity* player, uint8_t weapon)
{
	static_cast<IPlayer*>(player)->removeWeapon(weapon);
}

void Player_SetWeaponAmmo(IEntity* player, WeaponSlotData data)
{
	static_cast<IPlayer*>(player)->setWeaponAmmo(data);
}

const WeaponSlotData* Player_GetWeapons(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getWeapons().data();
}

CWeaponSlotData Player_GetWeaponSlot(IEntity* player, int slot)
{
	auto data = static_cast<IPlayer*>(player)->getWeaponSlot(slot);
	return CWeaponSlotData { data.id, data.ammo };
}

void Player_ResetWeapons(IEntity* player)
{
	static_cast<IPlayer*>(player)->resetWeapons();
}

void Player_SetArmedWeapon(IEntity* player, uint32_t weapon)
{
	static_cast<IPlayer*>(player)->setArmedWeapon(weapon);
}

uint32_t Player_GetArmedWeapon(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getArmedWeapon();
}

uint32_t Player_GetArmedWeaponAmmo(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getArmedWeaponAmmo();
}

void Player_SetShopName(IEntity* player, const char* name)
{
	static_cast<IPlayer*>(player)->setShopName(name);
}

const char* Player_GetShopName(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getShopName().data();
}

void Player_SetDrunkLevel(IEntity* player, int level)
{
	static_cast<IPlayer*>(player)->setDrunkLevel(level);
}

int Player_GetDrunkLevel(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getDrunkLevel();
}

void Player_SetColour(IEntity* player, uint32_t argb)
{
	static_cast<IPlayer*>(player)->setColour(Colour::FromARGB(argb));
}

uint32_t Player_GetColour(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getColour().ARGB();
}

void Player_SetOtherColour(IEntity* player, IEntity* other, uint32_t argb)
{
	static_cast<IPlayer*>(player)->setOtherColour(*static_cast<IPlayer*>(other), Colour::FromARGB(argb));
}

bool Player_GetOtherColour(const IEntity* player, IEntity* other, uint32_t* argb)
{
	Colour colour;
	auto ret = static_cast<const IPlayer*>(player)->getOtherColour(*static_cast<IPlayer*>(other), colour);
	*argb = colour.ARGB();
	return ret;
}

void Player_SetControllable(IEntity* player, bool controllable)
{
	static_cast<IPlayer*>(player)->setControllable(controllable);
}

bool Player_GetControllable(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getControllable();
}

void Player_SetSpectating(IEntity* player, bool spectating)
{
	static_cast<IPlayer*>(player)->setSpectating(spectating);
}

void Player_SetWantedLevel(IEntity* player, unsigned level)
{
	static_cast<IPlayer*>(player)->setWantedLevel(level);
}

unsigned Player_GetWantedLevel(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getWantedLevel();
}

void Player_PlaySound(IEntity* player, uint32_t sound, Vector3 pos)
{
	static_cast<IPlayer*>(player)->playSound(sound, pos);
}

uint32_t Player_LastPlayedSound(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->lastPlayedSound();
}

void Player_PlayAudio(IEntity* player, const char* url, bool usePos, Vector3 pos, float distance)
{
	static_cast<IPlayer*>(player)->playAudio(url, usePos, pos, distance);
}

bool Player_PlayerCrimeReport(IEntity* player, IEntity* suspect, int crime)
{
	return static_cast<IPlayer*>(player)->playerCrimeReport(*static_cast<IPlayer*>(suspect), crime);
}

void Player_StopAudio(IEntity* player)
{
	static_cast<IPlayer*>(player)->stopAudio();
}

const char* Player_LastPlayedAudio(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->lastPlayedAudio().data();
}

void Player_CreateExplosion(IEntity* player, Vector3 vec, int type, float radius)
{
	static_cast<IPlayer*>(player)->createExplosion(vec, type, radius);
}

void Player_SendDeathMessage(IEntity* player, IEntity* killed, IEntity* killer, int weapon)
{
	static_cast<IPlayer*>(player)->sendDeathMessage(*static_cast<IPlayer*>(killed), static_cast<IPlayer*>(killer), weapon);
}

void Player_SendEmptyDeathMessage(IEntity* player)
{
	static_cast<IPlayer*>(player)->sendEmptyDeathMessage();
}

void Player_RemoveDefaultObjects(IEntity* player, unsigned model, Vector3 pos, float radius)
{
	static_cast<IPlayer*>(player)->removeDefaultObjects(model, pos, radius);
}

void Player_ForceClassSelection(IEntity* player)
{
	static_cast<IPlayer*>(player)->forceClassSelection();
}

void Player_SetMoney(IEntity* player, int money)
{
	static_cast<IPlayer*>(player)->setMoney(money);
}

void Player_GiveMoney(IEntity* player, int money)
{
	static_cast<IPlayer*>(player)->giveMoney(money);
}

void Player_ResetMoney(IEntity* player)
{
	static_cast<IPlayer*>(player)->resetMoney();
}

int Player_GetMoney(IEntity* player)
{
	return static_cast<IPlayer*>(player)->getMoney();
}

void Player_SetMapIcon(IEntity* player, int id, Vector3 pos, int type, uint32_t argb, MapIconStyle style)
{
	static_cast<IPlayer*>(player)->setMapIcon(id, pos, type, Colour::FromARGB(argb), style);
}

void Player_UnsetMapIcon(IEntity* player, int id)
{
	static_cast<IPlayer*>(player)->unsetMapIcon(id);
}

void Player_UseStuntBonuses(IEntity* player, bool enable)
{
	static_cast<IPlayer*>(player)->useStuntBonuses(enable);
}

void Player_ToggleOtherNameTag(IEntity* player, IEntity* other, bool toggle)
{
	static_cast<IPlayer*>(player)->toggleOtherNameTag(*static_cast<IPlayer*>(other), toggle);
}

void Player_SetTime(IEntity* player, int64_t hr, int64_t min)
{
	static_cast<IPlayer*>(player)->setTime(Hours(hr), Minutes(min));
}

void Player_GetTime(const IEntity* player, int64_t* hours, int64_t* minutes)
{
	auto pair = static_cast<const IPlayer*>(player)->getTime();
	*hours = pair.first.count();
	*minutes = pair.second.count();
}

void Player_UseClock(IEntity* player, bool enable)
{
	static_cast<IPlayer*>(player)->useClock(enable);
}

bool Player_HasClock(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->hasClock();
}

void Player_UseWidescreen(IEntity* player, bool enable)
{
	static_cast<IPlayer*>(player)->useWidescreen(enable);
}

bool Player_HasWidescreen(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->hasWidescreen();
}

void Player_SetTransform(IEntity* player, Vector3 vec3)
{
	static_cast<IPlayer*>(player)->setTransform(GTAQuat(vec3));
}

void Player_SetHealth(IEntity* player, float health)
{
	static_cast<IPlayer*>(player)->setHealth(health);
}

float Player_GetHealth(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getHealth();
}

void Player_SetScore(IEntity* player, int score)
{
	static_cast<IPlayer*>(player)->setScore(score);
}

int Player_GetScore(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getScore();
}

void Player_SetArmour(IEntity* player, float armour)
{
	static_cast<IPlayer*>(player)->setArmour(armour);
}

float Player_GetArmour(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getArmour();
}

void Player_SetGravity(IEntity* player, float gravity)
{
	static_cast<IPlayer*>(player)->setGravity(gravity);
}

float Player_GetGravity(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getGravity();
}

void Player_SetWorldTime(IEntity* player, uint64_t time)
{
	static_cast<IPlayer*>(player)->setWorldTime(Hours(time));
}

void Player_ApplyAnimation(IEntity* player, const AnimationData& animation, PlayerAnimationSyncType syncType)
{
	static_cast<IPlayer*>(player)->applyAnimation(animation, syncType);
}

void Player_ClearAnimations(IEntity* player, PlayerAnimationSyncType syncType)
{
	static_cast<IPlayer*>(player)->clearAnimations(syncType);
}

PlayerAnimationData Player_GetAnimationData(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getAnimationData();
}

PlayerSurfingData Player_GetSurfingData(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getSurfingData();
}

void Player_StreamInForPlayer(IEntity* player, IEntity* other)
{
	static_cast<IPlayer*>(player)->streamInForPlayer(*static_cast<IPlayer*>(other));
}

bool Player_IsStreamedInForPlayer(const IEntity* player, const IEntity* other)
{
	return static_cast<const IPlayer*>(player)->isStreamedInForPlayer(*static_cast<const IPlayer*>(other));
}

void Player_StreamOutForPlayer(IEntity* player, IEntity* other)
{
	static_cast<IPlayer*>(player)->streamOutForPlayer(*static_cast<IPlayer*>(other));
}

std::size_t Player_GetStreamedPlayers(IEntity* player, IEntity*** streamedPlayers)
{
	auto set = static_cast<IPlayer*>(player)->streamedForPlayers();
	auto size = set.size();
	std::size_t idx = 0;
	*streamedPlayers = new IEntity*[size];
	for (auto& i : set)
	{
		*(*streamedPlayers + idx++) = static_cast<IEntity*>(i);
	}
	return size;
}

PlayerState Player_GetState(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getState();
}

void Player_SetTeam(IEntity* player, int team)
{
	static_cast<IPlayer*>(player)->setTeam(team);
}

int Player_GetTeam(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getTeam();
}

void Player_SetSkin(IEntity* player, int skin, bool send)
{
	static_cast<IPlayer*>(player)->setSkin(skin, send);
}

int Player_GetSkin(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getSkin();
}

void Player_SetChatBubble(IEntity* player, const char* text, uint32_t argb, float drawDist, int64_t expireMs)
{
	static_cast<IPlayer*>(player)->setChatBubble(text, Colour::FromARGB(argb), drawDist, Milliseconds(expireMs));
}

void Player_SendClientMessage(IEntity* player, uint32_t argb, const char* message)
{
	static_cast<IPlayer*>(player)->sendClientMessage(Colour::FromARGB(argb), message);
}

void Player_SendChatMessage(IEntity* player, IEntity* sender, const char* message)
{
	static_cast<IPlayer*>(player)->sendChatMessage(*static_cast<IPlayer*>(sender), message);
}

void Player_SendCommand(IEntity* player, const char* message)
{
	static_cast<IPlayer*>(player)->sendCommand(message);
}

void Player_SendGameText(IEntity* player, const char* message, int64_t timeMs, int style)
{
	static_cast<IPlayer*>(player)->sendGameText(message, Milliseconds(timeMs), style);
}

void Player_HideGameText(IEntity* player, int style)
{
	static_cast<IPlayer*>(player)->hideGameText(style);
}

bool Player_HasGameText(IEntity* player, int style)
{
	return static_cast<IPlayer*>(player)->hasGameText(style);
}

const char* Player_GetGameText(IEntity* player, int style, int64_t* timeMs, int64_t* remainingMs)
{
	StringView message_;
	Milliseconds time_, remaining_;
	auto ret = static_cast<IPlayer*>(player)->getGameText(style, message_, time_, remaining_);
	*timeMs = time_.count();
	*remainingMs = remaining_.count();
	return message_.data();
}

void Player_SetWeather(IEntity* player, int weatherID)
{
	static_cast<IPlayer*>(player)->setWeather(weatherID);
}

int Player_GetWeather(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getWeather();
}

void Player_SetWorldBounds(IEntity* player, Vector4 coords)
{
	static_cast<IPlayer*>(player)->setWorldBounds(coords);
}

Vector4 Player_GetWorldBounds(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getWorldBounds();
}

void Player_SetFightingStyle(IEntity* player, PlayerFightingStyle style)
{
	static_cast<IPlayer*>(player)->setFightingStyle(style);
}

PlayerFightingStyle Player_GetFightingStyle(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getFightingStyle();
}

void Player_SetSkillLevel(IEntity* player, PlayerWeaponSkill skill, int level)
{
	static_cast<IPlayer*>(player)->setSkillLevel(skill, level);
}

void Player_SetAction(IEntity* player, PlayerSpecialAction action)
{
	static_cast<IPlayer*>(player)->setAction(action);
}

PlayerSpecialAction Player_GetAction(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getAction();
}

void Player_SetVelocity(IEntity* player, Vector3 velocity)
{
	static_cast<IPlayer*>(player)->setVelocity(velocity);
}

Vector3 Player_GetVelocity(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getVelocity();
}

void Player_SetInterior(IEntity* player, unsigned interior)
{
	static_cast<IPlayer*>(player)->setInterior(interior);
}

unsigned Player_GetInterior(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getInterior();
}

const PlayerKeyData Player_GetKeyData(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getKeyData();
}

const uint16_t* Player_GetSkillLevels(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getSkillLevels().data();
}

const PlayerAimData Player_GetAimData(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getAimData();
}

const PlayerBulletData Player_GetBulletData(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getBulletData();
}

void Player_UseCameraTargeting(IEntity* player, bool enable)
{
	static_cast<IPlayer*>(player)->useCameraTargeting(enable);
}

bool Player_HasCameraTargeting(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->hasCameraTargeting();
}

void Player_RemoveFromVehicle(IEntity* player, bool force)
{
	static_cast<IPlayer*>(player)->removeFromVehicle(force);
}

IEntity* Player_GetCameraTargetPlayer(IEntity* player)
{
	return static_cast<IPlayer*>(player)->getCameraTargetPlayer();
}

IVehicle* Player_GetCameraTargetVehicle(IEntity* player)
{
	return static_cast<IPlayer*>(player)->getCameraTargetVehicle();
}

IObject* Player_GetCameraTargetObject(IEntity* player)
{
	return static_cast<IPlayer*>(player)->getCameraTargetObject();
}

IActor* Player_GetCameraTargetActor(IEntity* player)
{
	return static_cast<IPlayer*>(player)->getCameraTargetActor();
}

IEntity* Player_GetTargetPlayer(IEntity* player)
{
	return static_cast<IPlayer*>(player)->getTargetPlayer();
}

IActor* Player_GetTargetActor(IEntity* player)
{
	return static_cast<IPlayer*>(player)->getTargetActor();
}

void Player_SetRemoteVehicleCollisions(IEntity* player, bool collide)
{
	static_cast<IPlayer*>(player)->setRemoteVehicleCollisions(collide);
}

void Player_SpectatePlayer(IEntity* player, IEntity* target, PlayerSpectateMode mode)
{
	static_cast<IPlayer*>(player)->spectatePlayer(*static_cast<IPlayer*>(target), mode);
}

void Player_SpectateVehicle(IEntity* player, IVehicle* target, PlayerSpectateMode mode)
{
	static_cast<IPlayer*>(player)->spectateVehicle(*target, mode);
}

const PlayerSpectateData Player_GetSpectateData(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getSpectateData();
}

void Player_SendClientCheck(IEntity* player, int actionType, int address, int offset, int count)
{
	static_cast<IPlayer*>(player)->sendClientCheck(actionType, address, offset, count);
}

void Player_ToggleGhostMode(IEntity* player, bool toggle)
{
	static_cast<IPlayer*>(player)->toggleGhostMode(toggle);
}

bool Player_IsGhostModeEnabled(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->isGhostModeEnabled();
}

int Player_GetDefaultObjectsRemoved(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getDefaultObjectsRemoved();
}

bool Player_GetKickStatus(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->getKickStatus();
}

void Player_ClearTasks(IEntity* player, PlayerAnimationSyncType syncType)
{
	static_cast<IPlayer*>(player)->clearTasks(syncType);
}

void Player_AllowWeapons(IEntity* player, bool allow)
{
	static_cast<IPlayer*>(player)->allowWeapons(allow);
}

bool Player_AreWeaponsAllowed(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->areWeaponsAllowed();
}

void Player_AllowTeleport(IEntity* player, bool allow)
{
	static_cast<IPlayer*>(player)->allowTeleport(allow);
}

bool Player_IsTeleportAllowed(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->isTeleportAllowed();
}

bool Player_IsUsingOfficialClient(const IEntity* player)
{
	return static_cast<const IPlayer*>(player)->isUsingOfficialClient();
}

#include <cstring>

#include <Server/Components/Classes/classes.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Server/Components/Actors/actors.hpp"
#include "Server/Components/Objects/objects.hpp"
#include "Server/Components/TextDraws/textdraws.hpp"
#include "component.hpp"
#include "gtaquat.hpp"
#include "network.hpp"
#include "player.hpp"
#include "types.hpp"
#include "src/c-api/player.hpp"
#include "src/component.hpp"
#include "values.hpp"

#define GET_PLAYER_CHECKED(player_output, playerid)                                               \
	IPlayer* player_output = MainComponent::getInstance()->getCore()->getPlayers().get(playerid); \
	if (player_output == nullptr)

bool Player_SetSpawnInfo(int playerid, int team, int skin, float x, float y, float z, float rotation, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = queryExtension<IPlayerClassData>(player);
	if (!data)
	{
		return false;
	}
	WeaponSlots slots;
	auto slot_data = WeaponSlotData(weapon1, weapon1_ammo);
	slots[slot_data.slot()] = slot_data;
	slot_data = WeaponSlotData(weapon2, weapon2_ammo);
	slots[slot_data.slot()] = slot_data;
	slot_data = WeaponSlotData(weapon3, weapon3_ammo);
	slots[slot_data.slot()] = slot_data;
	data->setSpawnInfo(PlayerClass(
		skin,
		team,
		glm::vec3(x, y, z),
		rotation,
		slots));
	return true;
}

bool Player_Spawn(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->spawn();
	return true;
}

bool Player_SetPosition(int playerid, float x, float y, float z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setPosition(glm::vec3(x, y, z));
	return true;
}

bool Player_SetPositionFindZ(int playerid, float x, float y, float z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setPositionFindZ(glm::vec3(x, y, z));
	return true;
}

bool Player_GetPosition(int playerid, float* x, float* y, float* z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto vec3 = player->getPosition();
	*x = vec3.x;
	*y = vec3.y;
	*z = vec3.z;
	return true;
}

bool Player_SetFacingAngle(int playerid, float angle)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setRotation(GTAQuat(glm::vec3(0, 0, angle)));
	return true;
}

bool Player_GetFacingAngle(int playerid, float* angle)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	*angle = player->getRotation().ToEuler().z;
	return true;
}
// bool Player_IsInRangeOfPoint(int playerid, float range, float x, float y, float z)
// {
// 	GET_PLAYER_CHECKED(player, playerid)
// 	{
// 		return false;
// 	}
// 	return glm::distance(player->getPosition(), glm::vec3(x, y, z)) <= range;
// }
// float Player_GetDistanceFromPoint(int playerid, float x, float y, float z)
// {
// 	GET_PLAYER_CHECKED(player, playerid)
// 	{
// 		return false;
// 	}
// 	return true;
// }

bool Player_IsStreamedIn(int playerid, int forplayerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}

	GET_PLAYER_CHECKED(other, forplayerid)
	{
		return false;
	}
	player->isStreamedInForPlayer(*other);
	return true;
}

bool Player_SetInterior(int playerid, unsigned int interiorid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setInterior(interiorid);
	return true;
}

unsigned int Player_GetInterior(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getInterior();
}

bool Player_SetHealth(int playerid, float health)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setHealth(health);
	return true;
}

bool Player_GetHealth(int playerid, float* health)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	*health = player->getHealth();
	return true;
}

bool Player_SetArmour(int playerid, float armour)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setArmour(armour);
	return true;
}

bool Player_GetArmour(int playerid, float* armour)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	*armour = player->getArmour();
	return true;
}

bool Player_SetAmmo(int playerid, int weaponid, int ammo)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setWeaponAmmo(WeaponSlotData(weaponid, ammo));
	return true;
}

int Player_GetAmmo(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getArmedWeaponAmmo();
}

int Player_GetWeaponState(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return PlayerWeaponState::PlayerWeaponState_Unknown;
	}
	return player->getAimData().weaponState;
}

int Player_GetTargetPlayer(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return INVALID_PLAYER_ID;
	}
	auto target = player->getTargetPlayer();
	return target ? target->getID() : INVALID_PLAYER_ID;
}

int Player_GetTargetActor(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return INVALID_ACTOR_ID;
	}
	auto target = player->getTargetActor();
	return target ? target->getID() : INVALID_ACTOR_ID;
}

bool Player_SetTeam(int playerid, int teamid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setTeam(teamid);
	return true;
}

int Player_GetTeam(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return TEAM_NONE;
	}
	return player->getTeam();
}

bool Player_SetScore(int playerid, int score)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setScore(score);
	return true;
}

int Player_GetScore(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getScore();
}

int Player_GetDrunkLevel(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getDrunkLevel();
}

bool Player_SetDrunkLevel(int playerid, int level)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setDrunkLevel(level);
	return true;
}

bool Player_SetColor(int playerid, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setColour(Colour(r, g, b, a));
	return true;
}

uint32_t Player_GetColor(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getColour().RGBA();
	return true;
}

bool Player_SetSkin(int playerid, int skinid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setSkin(skinid);
	return true;
}

int Player_GetSkin(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getSkin();
}

bool Player_GiveWeapon(int playerid, int weaponid, int ammo)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->giveWeapon(WeaponSlotData(weaponid, ammo));
	return true;
}

bool Player_ResetWeapons(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->resetWeapons();
	return true;
}

bool Player_SetArmedWeapon(int playerid, uint32_t weaponid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setArmedWeapon(weaponid);
	return true;
}

bool Player_GetWeaponData(int playerid, int slot, int* weapon, int* ammo)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = player->getWeaponSlot(slot);
	*weapon = data.id;
	*ammo = data.ammo;
	return true;
}

bool Player_GiveMoney(int playerid, int money)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->giveMoney(money);
	return true;
}

bool Player_ResetMoney(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->resetMoney();
	return true;
}

int Player_SetName(int playerid, const char* name)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return -1;
	}
	return player->setName(name);
}

int Player_GetMoney(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getMoney();
}

int Player_GetState(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return PlayerState::PlayerState_None;
	}
	return player->getState();
}

bool Player_GetIp(int playerid, char* ip, int size)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	HybridString<46> address;
	PeerAddress::ToString(player->getNetworkData().networkID.address, address);
	memcpy(ip, address.data(), size);
	return true;
}

unsigned int Player_GetPing(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getPing();
}

int Player_GetArmedWeapon(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getArmedWeapon();
}

bool Player_GetKeys(int playerid, int* keys, int* updown, int* leftright)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = player->getKeyData();
	*keys = data.keys;
	*updown = data.upDown;
	*leftright = data.leftRight;
	return true;
}

void Player_GetName(int playerid, char* name, int size)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return;
	}

	memcpy(name, player->getName().data(), size);
}

bool Player_SetTime(int playerid, int hour, int minute)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setTime(Hours(hour), Minutes(minute));
	return true;
}

bool Player_GetTime(int playerid, int* hour, int* minute)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto pair = player->getTime();
	std::chrono::duration(pair.first);
	*hour = pair.first.count();
	*minute = pair.second.count();
	return true;
}

bool Player_ToggleClock(int playerid, bool toggle)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->useClock(toggle);
	return true;
}

bool Player_SetWeather(int playerid, int weather)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setWeather(weather);
	return true;
}

bool Player_ForceClassSelection(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->forceClassSelection();
	return true;
}

bool Player_SetWantedLevel(int playerid, unsigned int level)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setWantedLevel(level);
	return true;
}

unsigned int Player_GetWantedLevel(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getWantedLevel();
}

bool Player_SetFightingStyle(int playerid, int style)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setFightingStyle(static_cast<PlayerFightingStyle>(style));
	return true;
}

int Player_GetFightingStyle(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getFightingStyle();
}

bool Player_SetVelocity(int playerid, float x, float y, float z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setVelocity(glm::vec3(x, y, z));
	return true;
}

bool Player_GetVelocity(int playerid, float* x, float* y, float* z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto vec3 = player->getVelocity();
	*x = vec3.x;
	*y = vec3.y;
	*z = vec3.z;
	return true;
}

bool Player_PlayCrimeReport(int playerid, int suspectid, int crime)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}

	GET_PLAYER_CHECKED(suspect, suspectid)
	{
		return false;
	}
	return player->playerCrimeReport(*suspect, crime);
}

bool Player_PlayAudioStream(int playerid, const char* url, float x, float y, float z, float distance, bool usepos)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->playAudio(url, usepos, glm::vec3(x, y, z), distance);
	return true;
}

bool Player_StopAudioStream(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->stopAudio();
	return true;
}

bool Player_SetShopName(int playerid, const char* shopname)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setShopName(shopname);
	return true;
}

bool Player_SetSkillLevel(int playerid, int skill, int level)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setSkillLevel(static_cast<PlayerWeaponSkill>(skill), level);
	return true;
}

int Player_GetSurfingVehicleID(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return INVALID_VEHICLE_ID;
	}
	auto data = player->getSurfingData();
	if (data.type != PlayerSurfingData::Type::Vehicle)
	{
		return INVALID_VEHICLE_ID;
	}
	return data.ID;
}

int Player_GetSurfingObjectID(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return INVALID_OBJECT_ID;
	}
	auto data = player->getSurfingData();
	if (data.type != PlayerSurfingData::Type::Object
		&& data.type != PlayerSurfingData::Type::PlayerObject)
	{
		return INVALID_OBJECT_ID;
	}
	return data.ID;
}

bool Player_RemoveBuilding(int playerid, int modelid, float x, float y, float z, float radius)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->removeDefaultObjects(modelid, glm::vec3(x, y, z), radius);
	return true;
}

bool Player_GetLastShotVectors(int playerid, float* origin_x, float* origin_y, float* origin_z, float* hit_x, float* hit_y, float* hit_z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = player->getBulletData();
	*origin_x = data.origin.x;
	*origin_y = data.origin.y;
	*origin_z = data.origin.z;
	*hit_x = data.hitPos.x;
	*hit_y = data.hitPos.y;
	*hit_z = data.hitPos.z;
	return true;
}

bool Player_SetAttachedObject(int playerid, int index, int modelid, int bone, float offset_x, float offset_y, float offset_z, float rot_x, float rot_y, float rot_z, float scale_x, float scale_y, float scale_z, int mat_color1, int mat_color2)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto object_data = queryExtension<IPlayerObjectData>(player);
	if (!object_data)
	{
		return false;
	}
	object_data->setAttachedObject(
		index,
		ObjectAttachmentSlotData {
			modelid,
			bone,
			glm::vec3(offset_x, offset_y, offset_z),
			glm::vec3(rot_x, rot_y, rot_z),
			glm::vec3(scale_x, scale_y, scale_z),
			Colour::FromRGBA(mat_color1),
			Colour::FromRGBA(mat_color2) });
	return true;
}

bool Player_RemoveAttachedObject(int playerid, int index)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto object_data = queryExtension<IPlayerObjectData>(player);
	if (!object_data)
	{
		return false;
	}
	object_data->removeAttachedObject(index);
	return true;
}

bool Player_IsAttachedObjectSlotUsed(int playerid, int index)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto object_data = queryExtension<IPlayerObjectData>(player);
	if (!object_data)
	{
		return false;
	}
	return object_data->hasAttachedObject(index);
}

bool Player_EditAttachedObject(int playerid, int index)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto object_data = queryExtension<IPlayerObjectData>(player);
	if (!object_data)
	{
		return false;
	}
	object_data->editAttachedObject(index);
	return true;
}

int Player_CreateTextDraw(int playerid, float x, float y, const char* text)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return INVALID_TEXTDRAW;
	}
	auto player_td_data = queryExtension<IPlayerTextDrawData>(player);
	if (!player_td_data)
	{
		return INVALID_TEXTDRAW;
	}
	return player_td_data->create(glm::vec2(x, y), text)->getID();
}

bool PlayerTextDrawDestroy(int playerid, int textId)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto player_td_data = queryExtension<IPlayerTextDrawData>(player);
	if (!player_td_data)
	{
		return false;
	}
	player_td_data->release(textId);
	return true;
}

bool PlayerTextDrawLetterSize(int playerid, int textId, float x, float y)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawTextSize(int playerid, int textId, float x, float y)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawAlignment(int playerid, int textId, int alignment)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawColor(int playerid, int textId, int color)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawUseBox(int playerid, int textId, bool use)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawBoxColor(int playerid, int textId, int color)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawSetShadow(int playerid, int textId, int size)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawSetOutline(int playerid, int textId, int size)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawBackgroundColor(int playerid, int textId, int color)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawFont(int playerid, int textId, int font)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawSetProportional(int playerid, int textId, bool set)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawSetSelectable(int playerid, int textId, bool set)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawShow(int playerid, int textId)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawHide(int playerid, int textId)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawSetString(int playerid, int textId, const char* text)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawSetPreviewModel(int playerid, int textId, int modelindex)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawSetPreviewRot(int playerid, int textId, float fRotX, float fRotY, float fRotZ, float fZoom)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool PlayerTextDrawSetPreviewVehCol(int playerid, int textId, int color1, int color2)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool Player_SetPVarInt(int playerid, const char* varname, int value)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

int Player_GetPVarInt(int playerid, const char* varname)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool Player_SetPVarString(int playerid, const char* varname, const char* value)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool Player_GetPVarString(int playerid, const char* varname, char* value, int size)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool Player_SetPVarFloat(int playerid, const char* varname, float value)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

float Player_GetPVarFloat(int playerid, const char* varname)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool Player_DeletePVar(int playerid, const char* varname)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

int Player_GetPVarsUpperIndex(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool Player_GetPVarNameAtIndex(int playerid, int index, char* varname, int size)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

int Player_GetPVarType(int playerid, const char* varname)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool Player_SetChatBubble(int playerid, const char* text, int color, float drawdistance, int expiretime)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

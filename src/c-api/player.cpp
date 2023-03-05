#include <cstring>

#include <Server/Components/Classes/classes.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Server/Components/Actors/actors.hpp"
#include "Server/Components/Checkpoints/checkpoints.hpp"
#include "Server/Components/Objects/objects.hpp"
#include "Server/Components/TextDraws/textdraws.hpp"
#include "Server/Components/Vehicles/vehicles.hpp"
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

#define GET_VEHICLE_CHECKED(vehicle_output, vehicleid)                                               \
	IVehicle* vehicle_output = MainComponent::getInstance()->getVehiclesComponent()->get(vehicleid); \
	if (vehicle_output == nullptr)

#define GET_OBJECT_CHECKED(object_output, objectid)                                              \
	IObject* object_output = MainComponent::getInstance()->getObjectsComponent()->get(objectid); \
	if (object_output == nullptr)

#define GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data_output, textid)                    \
	IPlayerTextDrawData* textdraw_data_output = queryExtension<IPlayerTextDrawData>(player); \
	if (textdraw_data_output == nullptr)

#define GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw_output, textid) \
	IPlayerTextDraw* textdraw_output = nullptr;                                           \
	for (auto it = textdraw_data->begin(), end = textdraw_data->end(); it != end; ++it)   \
	{                                                                                     \
		auto td = *it;                                                                    \
		if (td->getID() == textid)                                                        \
		{                                                                                 \
			textdraw_output = td;                                                         \
		}                                                                                 \
	}

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

bool Player_IsStreamedInFor(int playerid, int forplayerid)
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

bool Player_SetColor(int playerid, uint32_t argb)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setColour(Colour::FromARGB(argb));
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

bool Player_GetIp(int playerid, char* ip, std::size_t size)
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

void Player_GetName(int playerid, char* name, std::size_t size)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return;
	}
	player->getName().copy(name, size);
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

int Player_CreatePreviewModelTextDraw(int playerid, float x, float y, int model)
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
	return player_td_data->create(glm::vec2(x, y), model)->getID();
}

bool Player_DestroyTextDraw(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	for (auto it = textdraw_data->begin(), end = textdraw_data->end(); it != end; ++it)
	{
		auto td = *it;
		if (td->getID() == textid)
		{
			textdraw_data->release(std::distance(textdraw_data->begin(), it));
			return true;
		}
	}
	return false;
}

bool Player_GetTextDrawPosition(int playerid, int textid, float* x, float* y)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	auto vec2 = textdraw->getPosition();
	*x = vec2.x;
	*y = vec2.y;
	return true;
}

bool Player_GetTextDrawLetterSize(int playerid, int textid, float* x, float* y)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	auto vec2 = textdraw->getLetterSize();
	*x = vec2.x;
	*y = vec2.y;
	return true;
}

bool Player_GetTextDrawTextSize(int playerid, int textid, float* x, float* y)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	auto vec2 = textdraw->getTextSize();
	*x = vec2.x;
	*y = vec2.y;
	return true;
}

int Player_GetTextDrawAlignment(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return TextDrawAlignmentTypes::TextDrawAlignment_Default;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return TextDrawAlignmentTypes::TextDrawAlignment_Default;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return TextDrawAlignmentTypes::TextDrawAlignment_Default;
	}
	return textdraw->getAlignment();
}

uint32_t Player_GetTextDrawColor(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return 0;
	}
	return textdraw->getLetterColour().ARGB();
}

bool Player_TextDrawHasBox(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	return textdraw->hasBox();
}

uint32_t Player_GetTextDrawBoxColor(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	return textdraw->getBoxColour().ARGB();
}

int Player_GetTextDrawShadow(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return 0;
	}
	return textdraw->getShadow();
}

int Player_GetTextDrawOutline(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return 0;
	}
	return textdraw->getOutline();
}

uint32_t Player_GetTextDrawBackgroundColor(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return 0;
	}
	return textdraw->getBackgroundColour().ARGB();
}

int Player_GetTextDrawStyle(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return 0;
	}
	return textdraw->getStyle();
}

bool Player_IsTextDrawProportional(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	return textdraw->isProportional();
}

bool Player_IsTextDrawSelectable(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	return textdraw->isSelectable();
}

bool Player_GetTextDrawText(int playerid, int textid, char* text, std::size_t size)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->getText().copy(text, size);
	return true;
}

int Player_GetTextDrawPreviewModel(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return INVALID_OBJECT_MODEL_ID;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return INVALID_OBJECT_MODEL_ID;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return INVALID_OBJECT_MODEL_ID;
	}
	return textdraw->getPreviewModel();
}
bool Player_GetTextDrawPreviewRotation(int playerid, int textid, float* rot_x, float* rot_y, float* rot_z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	auto vec3 = player->getRotation().ToEuler();
	*rot_x = vec3.x;
	*rot_y = vec3.y;
	*rot_z = vec3.z;
	return true;
}

float Player_GetTextDrawPreviewZoom(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return 0;
	}
	return textdraw->getPreviewZoom();
}

int Player_GetTextDrawPreviewVehiclePrimaryColor(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return 0;
	}
	return textdraw->getPreviewVehicleColour().first;
}

int Player_GetTextDrawPreviewVehicleSecondaryColor(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return 0;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return 0;
	}
	return textdraw->getPreviewVehicleColour().second;
}

bool Player_SetTextDrawPosition(int playerid, int textid, float x, float y)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setPosition(glm::vec2(x, y));
	return true;
}

bool Player_SetTextDrawLetterSize(int playerid, int textid, float x, float y)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setLetterSize(glm::vec2(x, y));
	return true;
}

bool Player_SetTextDrawTextSize(int playerid, int textid, float x, float y)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setTextSize(glm::vec2(x, y));
	return true;
}

bool Player_SetTextDrawAlignment(int playerid, int textid, int alignment)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setAlignment(static_cast<TextDrawAlignmentTypes>(alignment));
	return true;
}

bool Player_SetTextDrawColor(int playerid, int textid, uint32_t argb)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setColour(Colour::FromARGB(argb));
	return true;
}

bool Player_SetTextDrawUseBox(int playerid, int textid, bool use)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->useBox(use);
	return true;
}

bool Player_SetTextDrawBoxColor(int playerid, int textid, uint32_t argb)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setBoxColour(Colour::FromARGB(argb));
	return true;
}

bool Player_SetTextDrawShadow(int playerid, int textid, int size)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setShadow(size);
	return true;
}

bool Player_SetTextDrawOutline(int playerid, int textid, int size)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setOutline(size);
	return true;
}

bool Player_SetTextDrawBackgroundColor(int playerid, int textid, uint32_t argb)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setBackgroundColour(Colour::FromARGB(argb));
	return true;
}

bool Player_SetTextDrawStyle(int playerid, int textid, int style)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setStyle(static_cast<TextDrawStyle>(style));
	return true;
}

bool Player_SetTextDrawProportional(int playerid, int textid, bool proportional)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setProportional(proportional);
	return true;
}

bool Player_SetTextDrawSelectable(int playerid, int textid, bool selectable)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setSelectable(selectable);
	return true;
}

bool Player_ShowTextDraw(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->show();
	return true;
}

bool Player_HideTextDraw(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->hide();
	return true;
}

bool Player_SetTextDrawText(int playerid, int textid, const char* text)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setText(text);
	return true;
}

bool Player_SetTextDrawPreviewModel(int playerid, int textid, int model)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setPreviewModel(model);
	return true;
}

bool Player_SetTextDrawPreviewRotation(int playerid, int textid, float rot_x, float rot_y, float rot_z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setPreviewRotation(glm::vec3(rot_x, rot_y, rot_z));
	return true;
}

bool Player_SetTextDrawPreviewZoom(int playerid, int textid, float zoom)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setPreviewZoom(zoom);
	return true;
}

bool Player_SetTextDrawPreviewVehiclePrimaryColor(int playerid, int textid, int color)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setPreviewVehicleColour(color, textdraw->getPreviewVehicleColour().second);
	return true;
}

bool Player_SetTextDrawPreviewVehicleSecondaryColor(int playerid, int textid, int color)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->setPreviewVehicleColour(textdraw->getPreviewVehicleColour().first, color);
	return true;
}

bool Player_RestreamTextDraw(int playerid, int textid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data, textid)
	{
		return false;
	}
	GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw, textid)
	{
		return false;
	}
	textdraw->restream();
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

bool Player_GetPVarString(int playerid, const char* varname, char* value, std::size_t size)
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

bool Player_GetPVarNameAtIndex(int playerid, int index, char* varname, std::size_t size)
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

bool Player_PutInVehicle(int playerid, int vehicleid, int seatid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_VEHICLE_CHECKED(vehicle, vehicleid)
	{
		return false;
	}
	vehicle->putPlayer(*player, seatid);
	return true;
}

int Player_GetVehicleID(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return INVALID_VEHICLE_ID;
	}
	auto data = queryExtension<IPlayerVehicleData>(player);
	if (data == nullptr)
	{
		return INVALID_VEHICLE_ID;
	}
	return data->getVehicle()->getID();
}

int Player_GetVehicleSeat(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return -1;
	}
	auto data = queryExtension<IPlayerVehicleData>(player);
	if (data == nullptr)
	{
		return -1;
	}
	return data->getSeat();
}

bool Player_RemoveFromVehicle(int playerid, bool force)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->removeFromVehicle(force);
	return true;
}

bool Player_ToggleControllable(int playerid, bool toggle)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setControllable(toggle);
	return true;
}

bool Player_PlaySound(int playerid, uint32_t soundid, float x, float y, float z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->playSound(soundid, glm::vec3(x, y, z));
	return true;
}

bool Player_ApplyAnimation(int playerid, const char* anim_lib, const char* anim_name, float delta, bool loop, bool lock_x, bool lock_y, bool freeze, int time, int sync_type)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->applyAnimation(
		AnimationData(delta, loop, lock_x, lock_y, freeze, time, anim_lib, anim_name), static_cast<PlayerAnimationSyncType>(sync_type));
	return true;
}

bool Player_ClearAnimations(int playerid, int sync_type)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->clearAnimations(static_cast<PlayerAnimationSyncType>(sync_type));
	return true;
}

uint16_t Player_GetAnimationIndex(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getAnimationData().ID;
}

int Player_GetSpecialAction(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return PlayerSpecialAction::SpecialAction_None;
	}
	return player->getAction();
}

bool Player_SetSpecialAction(int playerid, int actionid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setAction(static_cast<PlayerSpecialAction>(actionid));
	return true;
}

bool Player_DisableRemoteVehicleCollisions(int playerid, bool collide)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setRemoteVehicleCollisions(collide);
	return true;
}

bool Player_SetCheckpoint(int playerid, float x, float y, float z, float radius)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = queryExtension<IPlayerCheckpointData>(player);
	if (data == nullptr)
	{
		return false;
	}

	ICheckpointData& cp = data->getCheckpoint();
	cp.setPosition(glm::vec3(x, y, z));
	cp.setRadius(radius);
	cp.enable();
	return true;
}

bool Player_DisableCheckpoint(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = queryExtension<IPlayerCheckpointData>(player);
	if (data == nullptr)
	{
		return false;
	}
	data->getCheckpoint().disable();
	return true;
}

bool Player_SetRaceCheckpoint(int playerid, int type, float x, float y, float z, float nextx, float nexty, float nextz, float radius)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = queryExtension<IPlayerCheckpointData>(player);
	if (data == nullptr)
	{
		return false;
	}

	IRaceCheckpointData& cp = data->getRaceCheckpoint();
	cp.setPosition(glm::vec3(x, y, z));
	cp.setNextPosition(glm::vec3(nextx, nexty, nextz));
	cp.setRadius(radius);
	cp.setType(static_cast<RaceCheckpointType>(type));
	cp.enable();
	return true;
}

bool Player_DisableRaceCheckpoint(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = queryExtension<IPlayerCheckpointData>(player);
	if (data == nullptr)
	{
		return false;
	}
	data->getRaceCheckpoint().disable();
	return true;
}

bool Player_SetWorldBounds(int playerid, float x_max, float x_min, float y_max, float y_min)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setWorldBounds(glm::vec4(x_max, x_min, y_max, y_min));
	return true;
}

bool PLayer_SetMarkerFor(int playerid, int showplayerid, int color)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool Player_ShowNameTagFor(int playerid, int showplayerid, bool show)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_CHECKED(showplayer, showplayerid)
	{
		return false;
	}
	player->toggleOtherNameTag(*showplayer, show);
	return true;
}

bool Player_SetMapIcon(int playerid, int iconid, float x, float y, float z, int markertype, uint32_t argb, int style)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setMapIcon(iconid, glm::vec3(x, y, z), markertype, Colour::FromRGBA(argb), static_cast<MapIconStyle>(style));
	return true;
}

bool Player_RemoveMapIcon(int playerid, int iconid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->unsetMapIcon(iconid);
	return true;
}

bool Player_AllowTeleport(int playerid, bool allow)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->allowTeleport(allow);
	return true;
}

bool Player_SetCameraPos(int playerid, float x, float y, float z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setCameraPosition(glm::vec3(x, y, z));
	return true;
}

bool Player_SetCameraLookAt(int playerid, float x, float y, float z, int cut_type)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setCameraLookAt(glm::vec3(x, y, z), static_cast<PlayerCameraCutType>(cut_type));
	return true;
}

bool Player_SetCameraBehind(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setCameraBehind();
	return true;
}

bool Player_GetCameraPos(int playerid, float* x, float* y, float* z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto vec3 = player->getCameraPosition();
	*x = vec3.x;
	*y = vec3.y;
	*z = vec3.z;
	return true;
}

bool Player_GetCameraFrontVector(int playerid, float* x, float* y, float* z)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto vec3 = player->getCameraLookAt();
	*x = vec3.x;
	*y = vec3.y;
	*z = vec3.z;
	return true;
}

uint8_t Player_GetCameraMode(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return -1;
	}
	return player->getAimData().camMode;
}

bool Player_EnableCameraTarget(int playerid, bool enable)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->useCameraTargeting(enable);
	return true;
}

int Player_GetCameraTargetObject(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return INVALID_OBJECT_ID;
	}
	auto object = player->getCameraTargetObject();
	if (object == nullptr)
	{
		return INVALID_OBJECT_ID;
	}
	return object->getID();
}

int Player_GetCameraTargetVehicle(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return INVALID_VEHICLE_ID;
	}
	auto vehicle = player->getCameraTargetVehicle();
	if (vehicle == nullptr)
	{
		return INVALID_VEHICLE_ID;
	}
	return vehicle->getID();
}

int Player_GetCameraTargetPlayer(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return INVALID_PLAYER_ID;
	}
	auto target = player->getCameraTargetPlayer();
	if (target == nullptr)
	{
		return INVALID_PLAYER_ID;
	}
	return target->getID();
}

int Player_GetCameraTargetActor(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return INVALID_ACTOR_ID;
	}
	auto actor = player->getCameraTargetActor();
	if (actor == nullptr)
	{
		return INVALID_ACTOR_ID;
	}
	return actor->getID();
}

float Player_GetCameraAspectRatio(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getAimData().aspectRatio;
}

float Player_GetCameraZoom(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getAimData().camZoom;
}

bool Player_AttachCameraToObject(int playerid, int objectid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_OBJECT_CHECKED(object, objectid)
	{
		return false;
	}
	player->attachCameraToObject(*object);
	return true;
}

bool Player_InterpolateCameraPos(int playerid, float from_x, float from_y, float from_z, float to_x, float to_y, float to_z, int time, int cut_type)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->interpolateCameraPosition(glm::vec3(from_x, from_y, from_z), glm::vec3(to_x, to_y, to_z), time, static_cast<PlayerCameraCutType>(cut_type));
	return true;
}

bool Player_InterpolateCameraLookAt(int playerid, float from_x, float from_y, float from_z, float to_x, float to_y, float to_z, int time, int cut_type)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->interpolateCameraLookAt(glm::vec3(from_x, from_y, from_z), glm::vec3(to_x, to_y, to_z), time, static_cast<PlayerCameraCutType>(cut_type));
	return true;
}

bool Player_IsConnected(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	return true;
}

bool Player_IsInVehicle(int playerid, int vehicleid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = queryExtension<IPlayerVehicleData>(player);
	if (data == nullptr)
	{
		return false;
	}
	auto vehicle = data->getVehicle();
	return vehicle != nullptr && vehicle->getID() == vehicleid;
}

bool Player_IsInAnyVehicle(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = queryExtension<IPlayerVehicleData>(player);
	return data != nullptr && data->getVehicle() != nullptr;
}

bool Player_IsInCheckpoint(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = queryExtension<IPlayerCheckpointData>(player);
	return data->getCheckpoint().isPlayerInside();
}

bool Player_IsInRaceCheckpoint(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	auto data = queryExtension<IPlayerCheckpointData>(player);
	return data->getRaceCheckpoint().isPlayerInside();
}

bool Player_SetVirtualWorld(int playerid, int worldid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setVirtualWorld(worldid);
	return true;
}

int Player_GetVirtualWorld(int playerid)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return 0;
	}
	return player->getVirtualWorld();
}

bool Player_EnableStuntBonus(int playerid, bool enable)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->useStuntBonuses(enable);
	return true;
}

bool Player_ToggleSpectating(int playerid, bool toggle)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->setSpectating(toggle);
	return true;
}

bool Player_SpectatePlayer(int playerid, int targetplayerid, int mode)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_PLAYER_CHECKED(target, targetplayerid)
	{
		return false;
	}
	player->spectatePlayer(*target, static_cast<PlayerSpectateMode>(mode));
	return true;
}

bool Player_CreateExplosion(int playerid, float x, float y, float z, int type, float radius)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	player->createExplosion(glm::vec3(x, y, z), type, radius);
	return true;
}

bool Player_SpectateVehicle(int playerid, int targetvehicleid, int mode)
{
	GET_PLAYER_CHECKED(player, playerid)
	{
		return false;
	}
	GET_VEHICLE_CHECKED(vehicle, targetvehicleid)
	{
		return false;
	}
	player->spectateVehicle(*vehicle, static_cast<PlayerSpectateMode>(mode));
	return true;
}

// bool Player_StartRecordingData(int playerid, int recordtype, const char* recordname)
// {
// 	GET_PLAYER_CHECKED(player, playerid)
// 	{
// 		return false;
// 	}
// 	return true;
// }
// bool Player_StopRecordingData(int playerid)
// {
// 	GET_PLAYER_CHECKED(player, playerid)
// 	{
// 		return false;
// 	}
// 	return true;
// }
// bool GetAnimationName(int index, char* anim_lib, int anim_lib_size, char* anim_name, int anim_name_size)
// {
// 	GET_PLAYER_CHECKED(player, playerid)
// 	{
// 		return false;
// 	}
// 	return true;
// }

#pragma once
#include <player.hpp>
#include "src/utils/export.hpp"

DLL_EXPORT bool Player_SetSpawnInfo(int playerid, int team, int skin, float x, float y, float z, float rotation, int weapon1, int weapon1_ammo, int weapon2, int weapon2_ammo, int weapon3, int weapon3_ammo);
DLL_EXPORT bool Player_Spawn(int playerid);
DLL_EXPORT bool Player_SetPosition(int playerid, float x, float y, float z);
DLL_EXPORT bool Player_SetPositionFindZ(int playerid, float x, float y, float z);
DLL_EXPORT bool Player_GetPosition(int playerid, float* x, float* y, float* z);
DLL_EXPORT bool Player_SetFacingAngle(int playerid, float angle);
DLL_EXPORT bool Player_GetFacingAngle(int playerid, float* angle);
// DLL_EXPORT bool Player_IsInRangeOfPoint(int playerid, float range, float x, float y, float z);
// DLL_EXPORT float Player_GetDistanceFromPoint(int playerid, float x, float y, float z);
DLL_EXPORT bool Player_IsStreamedIn(int playerid, int forplayerid);
DLL_EXPORT bool Player_SetInterior(int playerid, unsigned int interiorid);
DLL_EXPORT unsigned int Player_GetInterior(int playerid);
DLL_EXPORT bool Player_SetHealth(int playerid, float health);
DLL_EXPORT bool Player_GetHealth(int playerid, float* health);
DLL_EXPORT bool Player_SetArmour(int playerid, float armour);
DLL_EXPORT bool Player_GetArmour(int playerid, float* armour);
DLL_EXPORT bool Player_SetAmmo(int playerid, int weaponid, int ammo);
DLL_EXPORT int Player_GetAmmo(int playerid);
DLL_EXPORT int Player_GetWeaponState(int playerid);
DLL_EXPORT int Player_GetTargetPlayer(int playerid);
DLL_EXPORT int Player_GetTargetActor(int playerid);
DLL_EXPORT bool Player_SetTeam(int playerid, int teamid);
DLL_EXPORT int Player_GetTeam(int playerid);
DLL_EXPORT bool Player_SetScore(int playerid, int score);
DLL_EXPORT int Player_GetScore(int playerid);
DLL_EXPORT int Player_GetDrunkLevel(int playerid);
DLL_EXPORT bool Player_SetDrunkLevel(int playerid, int level);
DLL_EXPORT bool Player_SetColor(int playerid, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
DLL_EXPORT uint32_t Player_GetColour(int playerid);
DLL_EXPORT bool Player_SetSkin(int playerid, int skinid);
DLL_EXPORT int Player_GetSkin(int playerid);
DLL_EXPORT bool Player_GiveWeapon(int playerid, int weaponid, int ammo);
DLL_EXPORT bool Player_ResetWeapons(int playerid);
DLL_EXPORT bool Player_SetArmedWeapon(int playerid, uint32_t weaponid);
DLL_EXPORT bool Player_GetWeaponData(int playerid, int slot, int* weapon, int* ammo);
DLL_EXPORT bool Player_GiveMoney(int playerid, int money);
DLL_EXPORT bool Player_ResetMoney(int playerid);
DLL_EXPORT int Player_SetName(int playerid, const char* name);
DLL_EXPORT int Player_GetMoney(int playerid);
DLL_EXPORT int Player_GetState(int playerid);
DLL_EXPORT bool Player_GetIp(int playerid, char* ip, int size);
DLL_EXPORT unsigned int Player_GetPing(int playerid);
DLL_EXPORT int Player_GetArmedWeapon(int playerid);
DLL_EXPORT bool Player_GetKeys(int playerid, int* keys, int* updown, int* leftright);
DLL_EXPORT void Player_GetName(int playerid, char* name, int size);
DLL_EXPORT bool Player_SetTime(int playerid, int hour, int minute);
DLL_EXPORT bool Player_GetTime(int playerid, int* hour, int* minute);
DLL_EXPORT bool Player_ToggleClock(int playerid, bool toggle);
DLL_EXPORT bool Player_SetWeather(int playerid, int weather);
DLL_EXPORT bool Player_ForceClassSelection(int playerid);
DLL_EXPORT bool Player_SetWantedLevel(int playerid, unsigned int level);
DLL_EXPORT unsigned int Player_GetWantedLevel(int playerid);
DLL_EXPORT bool Player_SetFightingStyle(int playerid, int style);
DLL_EXPORT int Player_GetFightingStyle(int playerid);
DLL_EXPORT bool Player_SetVelocity(int playerid, float x, float y, float z);
DLL_EXPORT bool Player_GetVelocity(int playerid, float* x, float* y, float* z);
DLL_EXPORT bool Player_PlayCrimeReport(int playerid, int suspectid, int crime);
DLL_EXPORT bool Player_PlayAudioStream(int playerid, const char* url, float posX = 0.0, float posY = 0.0, float posZ = 0.0, float distance = 50.0, bool usepos = false);
DLL_EXPORT bool Player_StopAudioStream(int playerid);
DLL_EXPORT bool Player_SetShopName(int playerid, const char* shopname);
DLL_EXPORT bool Player_SetSkillLevel(int playerid, int skill, int level);
DLL_EXPORT int Player_GetSurfingVehicleID(int playerid);
DLL_EXPORT int Player_GetSurfingObjectID(int playerid);
DLL_EXPORT bool Player_RemoveBuilding(int playerid, int modelid, float fX, float fY, float fZ, float fRadius);
DLL_EXPORT bool Player_GetLastShotVectors(int playerid, float* fOriginX, float* fOriginY, float* fOriginZ, float* fHitPosX, float* fHitPosY, float* fHitPosZ);
DLL_EXPORT bool Player_SetAttachedObject(int playerid, int index, int modelid, int bone, float fOffsetX = 0.0, float fOffsetY = 0.0, float fOffsetZ = 0.0, float fRotX = 0.0, float fRotY = 0.0, float fRotZ = 0.0, float fScaleX = 1.0, float fScaleY = 1.0, float fScaleZ = 1.0, int materialcolor1 = 0, int materialcolor2 = 0);
DLL_EXPORT bool Player_RemoveAttachedObject(int playerid, int index);
DLL_EXPORT bool Player_IsAttachedObjectSlotUsed(int playerid, int index);
DLL_EXPORT bool Player_EditAttachedObject(int playerid, int index);
DLL_EXPORT int Player_CreateTextDraw(int playerid, float x, float y, const char* text);
DLL_EXPORT bool PlayerTextDrawDestroy(int playerid, int textId);
DLL_EXPORT bool PlayerTextDrawLetterSize(int playerid, int textId, float x, float y);
DLL_EXPORT bool PlayerTextDrawTextSize(int playerid, int textId, float x, float y);
DLL_EXPORT bool PlayerTextDrawAlignment(int playerid, int textId, int alignment);
DLL_EXPORT bool PlayerTextDrawColor(int playerid, int textId, int color);
DLL_EXPORT bool PlayerTextDrawUseBox(int playerid, int textId, bool use);
DLL_EXPORT bool PlayerTextDrawBoxColor(int playerid, int textId, int color);
DLL_EXPORT bool PlayerTextDrawSetShadow(int playerid, int textId, int size);
DLL_EXPORT bool PlayerTextDrawSetOutline(int playerid, int textId, int size);
DLL_EXPORT bool PlayerTextDrawBackgroundColor(int playerid, int textId, int color);
DLL_EXPORT bool PlayerTextDrawFont(int playerid, int textId, int font);
DLL_EXPORT bool PlayerTextDrawSetProportional(int playerid, int textId, bool set);
DLL_EXPORT bool PlayerTextDrawSetSelectable(int playerid, int textId, bool set);
DLL_EXPORT bool PlayerTextDrawShow(int playerid, int textId);
DLL_EXPORT bool PlayerTextDrawHide(int playerid, int textId);
DLL_EXPORT bool PlayerTextDrawSetString(int playerid, int textId, const char* text);
DLL_EXPORT bool PlayerTextDrawSetPreviewModel(int playerid, int textId, int modelindex);
DLL_EXPORT bool PlayerTextDrawSetPreviewRot(int playerid, int textId, float fRotX, float fRotY, float fRotZ, float fZoom = 1.0);
DLL_EXPORT bool PlayerTextDrawSetPreviewVehCol(int playerid, int textId, int color1, int color2);
DLL_EXPORT bool Player_SetPVarInt(int playerid, const char* varname, int value);
DLL_EXPORT int Player_GetPVarInt(int playerid, const char* varname);
DLL_EXPORT bool Player_SetPVarString(int playerid, const char* varname, const char* value);
DLL_EXPORT bool Player_GetPVarString(int playerid, const char* varname, char* value, int size);
DLL_EXPORT bool Player_SetPVarFloat(int playerid, const char* varname, float value);
DLL_EXPORT float Player_GetPVarFloat(int playerid, const char* varname);
DLL_EXPORT bool Player_DeletePVar(int playerid, const char* varname);
DLL_EXPORT int Player_GetPVarsUpperIndex(int playerid);
DLL_EXPORT bool Player_GetPVarNameAtIndex(int playerid, int index, char* varname, int size);
DLL_EXPORT int Player_GetPVarType(int playerid, const char* varname);
DLL_EXPORT bool Player_SetChatBubble(int playerid, const char* text, int color, float drawdistance, int expiretime);

DLL_EXPORT bool Player_PutInVehicle(int playerid, int vehicleid, int seatid);
DLL_EXPORT int Player_GetVehicleID(int playerid);
DLL_EXPORT int Player_GetVehicleSeat(int playerid);
DLL_EXPORT bool Player_RemoveFromVehicle(int playerid);
DLL_EXPORT bool Player_ToggleControllable(int playerid, bool toggle);
DLL_EXPORT bool Player_PlaySound(int playerid, int soundid, float x, float y, float z);
DLL_EXPORT bool Player_ApplyAnimation(int playerid, const char* animlib, const char* animname, float fDelta, bool loop, bool lockx, bool locky, bool freeze, int time, bool forcesync = false);
DLL_EXPORT bool Player_ClearAnimations(int playerid, bool forcesync = false);
DLL_EXPORT int Player_GetAnimationIndex(int playerid);
// DLL_EXPORT bool GetAnimationName(int index, char* animlib, int animlib_size, char* animname, int animname_size);
DLL_EXPORT int Player_GetSpecialAction(int playerid);
DLL_EXPORT bool Player_SetSpecialAction(int playerid, int actionid);
DLL_EXPORT bool Player_DisableRemoteVehicleCollisions(int playerid, bool disable);
DLL_EXPORT bool Player_SetCheckpoint(int playerid, float x, float y, float z, float size);
DLL_EXPORT bool Player_DisableCheckpoint(int playerid);
DLL_EXPORT bool Player_SetRaceCheckpoint(int playerid, int type, float x, float y, float z, float nextx, float nexty, float nextz, float size);
DLL_EXPORT bool Player_DisableRaceCheckpoint(int playerid);
DLL_EXPORT bool Player_SetWorldBounds(int playerid, float x_max, float x_min, float y_max, float y_min);
DLL_EXPORT bool Player_Player_SetMarkerFor(int playerid, int showplayerid, int color);
DLL_EXPORT bool Player_Player_ShowNameTagFor(int playerid, int showplayerid, bool show);
DLL_EXPORT bool Player_SetMapIcon(int playerid, int iconid, float x, float y, float z, int markertype, int color, int style = MapIconStyle::MapIconStyle_Local);
DLL_EXPORT bool Player_RemoveMapIcon(int playerid, int iconid);
DLL_EXPORT bool Player_AllowTeleport(int playerid, bool allow);
DLL_EXPORT bool Player_SetCameraPos(int playerid, float x, float y, float z);
DLL_EXPORT bool Player_SetCameraLookAt(int playerid, float x, float y, float z, int cut = PlayerCameraCutType::PlayerCameraCutType_Cut);
DLL_EXPORT bool Player_SetCameraBehind(int playerid);
DLL_EXPORT bool Player_GetCameraPos(int playerid, float* x, float* y, float* z);
DLL_EXPORT bool Player_GetCameraFrontVector(int playerid, float* x, float* y, float* z);
DLL_EXPORT int Player_GetCameraMode(int playerid);
DLL_EXPORT bool Player_EnableCameraTarget(int playerid, bool enable);
DLL_EXPORT int Player_GetCameraTargetObject(int playerid);
DLL_EXPORT int Player_GetCameraTargetVehicle(int playerid);
DLL_EXPORT int Player_Player_GetCameraTarget(int playerid);
DLL_EXPORT int Player_GetCameraTargetActor(int playerid);
DLL_EXPORT float Player_GetCameraAspectRatio(int playerid);
DLL_EXPORT float Player_GetCameraZoom(int playerid);
DLL_EXPORT bool Player_AttachCameraToObject(int playerid, int objectid);
DLL_EXPORT bool Player_InterpolateCameraPos(int playerid, float FromX, float FromY, float FromZ, float ToX, float ToY, float ToZ, int time, int cut = PlayerCameraCutType::PlayerCameraCutType_Cut);
DLL_EXPORT bool Player_InterpolateCameraLookAt(int playerid, float FromX, float FromY, float FromZ, float ToX, float ToY, float ToZ, int time, int cut = PlayerCameraCutType::PlayerCameraCutType_Cut);

DLL_EXPORT bool Player_IsConnected(int playerid);
DLL_EXPORT bool Player_IsInVehicle(int playerid, int vehicleid);
DLL_EXPORT bool Player_IsInAnyVehicle(int playerid);
DLL_EXPORT bool Player_IsInCheckpoint(int playerid);
DLL_EXPORT bool Player_IsInRaceCheckpoint(int playerid);

DLL_EXPORT bool Player_SetVirtualWorld(int playerid, int worldid);
DLL_EXPORT int Player_GetVirtualWorld(int playerid);

DLL_EXPORT bool Player_EnableStuntBonusFor(int playerid, bool enable);
// DLL_EXPORT bool Player_EnableStuntBonusForAll(bool enable);

DLL_EXPORT bool Player_ToggleSpectating(int playerid, bool toggle);
DLL_EXPORT bool Player_PlayerSpectate(int playerid, int targetplayerid, int mode = PlayerSpectateMode::PlayerSpectateMode_Normal);
DLL_EXPORT bool PlayerSpectateVehicle(int playerid, int targetvehicleid, int mode = PlayerSpectateMode::PlayerSpectateMode_Normal);

DLL_EXPORT bool Player_StartRecordingData(int playerid, int recordtype, const char* recordname);
DLL_EXPORT bool Player_StopRecordingData(int playerid);

DLL_EXPORT bool Player_CreateExplosionFor(int playerid, float X, float Y, float Z, int type, float Radius);

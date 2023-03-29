#pragma once

#include "entity.hpp"

struct EntityId
{
	IEntity* pointer;
	int id;
	EntityId(IEntity& entity)
		: pointer(&entity)
		, id(entity.getID())
	{
	}
	EntityId(IEntity* pointer)
		: pointer(pointer)
		, id(pointer == nullptr ? -1 : pointer->getID())
	{
	}
};

struct CWeaponSlotData
{
	uint8_t id;
	uint32_t ammo;
};

struct CVehicleParams
{
	int8_t engine;
	int8_t lights;
	int8_t alarm;
	int8_t doors;
	int8_t bonnet;
	int8_t boot;
	int8_t objective;
	int8_t siren;
	int8_t doorDriver;
	int8_t doorPassenger;
	int8_t doorBackLeft;
	int8_t doorBackRight;
	int8_t windowDriver;
	int8_t windowPassenger;
	int8_t windowBackLeft;
	int8_t windowBackRight;
};

struct CVehicleSpawnData
{
	int64_t respawnDelay;
	int modelID;
	Vector3 position;
	float zRotation;
	int colour1;
	int colour2;
	bool siren;
	int interior;
};

struct CVehicleDriverSyncPacket
{
	int PlayerID;
	uint16_t VehicleID;
	uint16_t LeftRight;
	uint16_t UpDown;
	uint16_t Keys;
	Vector3 Rotation;
	Vector3 Position;
	Vector3 Velocity;
	float Health;
	Vector2 PlayerHealthArmour;
	uint8_t Siren;
	uint8_t LandingGear;
	uint16_t TrailerID;
	bool HasTrailer;

	union
	{
		uint8_t AdditionalKeyWeapon;
		struct
		{
			uint8_t WeaponID : 6;
			uint8_t AdditionalKey : 2;
		};
	};

	union
	{
		uint32_t HydraThrustAngle;
		float TrainSpeed;
	};
};

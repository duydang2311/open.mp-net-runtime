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

#pragma once

#include "entity.hpp"

struct UnmanagedEntityId
{
	IEntity* pointer;
	int id;
	UnmanagedEntityId(IEntity& entity)
		: pointer(&entity)
		, id(entity.getID())
	{
	}
	UnmanagedEntityId(IEntity* pointer)
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

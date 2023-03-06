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
		, id(pointer->getID())
	{
	}
};

#pragma once

#include "entity.hpp"
#include "gtaquat.hpp"
#include "types.hpp"
#include "src/utils/export.hpp"

DLL_EXPORT Vector3 Entity_GetPosition(const IEntity* entity);
DLL_EXPORT void Entity_SetPosition(IEntity* entity, Vector3 position);
DLL_EXPORT Vector3 Entity_GetRotation(const IEntity* entity);
DLL_EXPORT void Entity_SetRotation(IEntity* entity, Vector3 rotation);
DLL_EXPORT int Entity_GetVirtualWorld(const IEntity* entity);
DLL_EXPORT void Entity_SetVirtualWorld(IEntity* entity, int vw);

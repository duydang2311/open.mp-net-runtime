#include "src/c-api/entity.hpp"
#include "src/component.hpp"

Vector3 Entity_GetPosition(const IEntity* entity)
{
	return entity->getPosition();
}

void Entity_SetPosition(IEntity* entity, Vector3 position)
{
	entity->setPosition(position);
}

Vector3 Entity_GetRotation(const IEntity* entity)
{
	return entity->getPosition();
}

void Entity_SetRotation(IEntity* entity, Vector3 rotation)
{
	entity->setRotation(GTAQuat(rotation));
}

int Entity_GetVirtualWorld(const IEntity* entity)
{
	return entity->getVirtualWorld();
}

void Entity_SetVirtualWorld(IEntity* entity, int vw)
{
	entity->setVirtualWorld(vw);
}

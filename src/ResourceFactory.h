#pragma once
#include "EntityFactory.h"
#include "ResourceType.h"
#include "ResourceEntity.h"

class ResourceFactory :public EntityFactory
{
public:
	ResourceFactory();
	~ResourceFactory();
	ResourceEntity* createResource(ResourceType type, Vector3 * pos);
};


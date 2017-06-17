#pragma once
#include "EntityFactory.h"
#include "ResourceType.h"
#include "ResourceEntity.h"
#include "SpacingType.h"

class ResourceFactory :public EntityFactory
{
public:
	ResourceFactory();
	~ResourceFactory();
	std::vector<ResourceEntity*>* create(unsigned number, ResourceType resourceType, Vector3* center, SpacingType spacing);
};


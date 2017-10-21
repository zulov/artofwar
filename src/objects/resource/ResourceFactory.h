#pragma once
#include "objects/EntityFactory.h"
#include "ResourceType.h"
#include "ResourceEntity.h"
#include "SpacingType.h"

class ResourceFactory :public EntityFactory
{
public:
	ResourceFactory();
	~ResourceFactory();
	std::vector<ResourceEntity*>* create(int id, Vector3* center, IntVector2 _bucketCords);
private:
	std::vector<ResourceEntity*>* resources;
};

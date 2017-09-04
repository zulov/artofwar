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
	std::vector<ResourceEntity*>* create(unsigned number, int id, Vector3* center, SpacingType spacing);
private:
	double spaceCoef = 7;
};

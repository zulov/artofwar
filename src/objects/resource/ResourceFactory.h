#pragma once
#include "objects/EntityFactory.h"
#include "ResourceEntity.h"
#include "simulation/SimulationObjectManager.h"


class ResourceFactory :public EntityFactory
{
public:
	ResourceFactory();
	~ResourceFactory();
	std::vector<ResourceEntity*>* create(int id, Vector3* center, IntVector2 _bucketCords);
	vector<ResourceEntity*>* load(dbload_resource_entities* resource);
private:
	std::vector<ResourceEntity*>* resources;
};

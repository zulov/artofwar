#pragma once
#include "objects/EntityFactory.h"
#include "ResourceEntity.h"


struct dbload_resource_entities;

class ResourceFactory :public EntityFactory
{
public:
	ResourceFactory();
	~ResourceFactory();
	std::vector<ResourceEntity*>* create(int id, Vector3* center, IntVector2 _bucketCords);
	std::vector<ResourceEntity*>* load(dbload_resource_entities* resource);
private:
	std::vector<ResourceEntity*>* resources;
};

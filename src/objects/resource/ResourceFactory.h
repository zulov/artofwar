#pragma once
#include "ResourceEntity.h"


struct dbload_resource_entities;

class ResourceFactory 
{
public:
	ResourceFactory();
	~ResourceFactory();
	std::vector<ResourceEntity*>* create(int id, Vector2& center, IntVector2 _bucketCords, int level);
	std::vector<ResourceEntity*>* load(dbload_resource_entities* resource);
private:
	std::vector<ResourceEntity*>* resources;
};

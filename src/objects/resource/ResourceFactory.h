#pragma once
#include "simulation/force/Force.h"


struct dbload_resource_entities;
class ResourceEntity;

class ResourceFactory 
{
public:
	ResourceFactory();
	~ResourceFactory();
	std::vector<ResourceEntity*>* create(int id, Urho3D::Vector2& center, Urho3D::IntVector2 _bucketCords, int level);
	std::vector<ResourceEntity*>* load(dbload_resource_entities* resource);
private:
	std::vector<ResourceEntity*>* resources;
};

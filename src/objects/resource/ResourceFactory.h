#pragma once
#include <vector>

namespace Urho3D {
	class IntVector2;
	class Vector2;
}

struct dbload_resource_entities;
class ResourceEntity;

class ResourceFactory 
{
public:
	ResourceFactory();
	~ResourceFactory();
	std::vector<ResourceEntity*>* create(int id, Urho3D::Vector2& center, Urho3D::IntVector2 _bucketCords, int level) const;
	std::vector<ResourceEntity*>* load(dbload_resource_entities* resource) const;
private:
	std::vector<ResourceEntity*>* resources;
};

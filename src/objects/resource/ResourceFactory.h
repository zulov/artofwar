#pragma once

namespace Urho3D {
	class IntVector2;
	class Vector2;
}

struct dbload_resource_entities;
class ResourceEntity;

class ResourceFactory {
public:
	ResourceFactory() = default;
	~ResourceFactory() = default;
	ResourceEntity* create(int id, Urho3D::IntVector2 bucketCords, int level) const;
	ResourceEntity* load(dbload_resource_entities* resource) const;
};

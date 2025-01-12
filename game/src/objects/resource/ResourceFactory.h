#pragma once

struct UId;

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
	ResourceEntity* create(int id, Urho3D::IntVector2 bucketCords, UId uid) const;
	ResourceEntity* load(dbload_resource_entities* resource) const;
};

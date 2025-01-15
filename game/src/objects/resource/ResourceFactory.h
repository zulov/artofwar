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
	void init(unsigned currentUnitUId);

	ResourceEntity* create(int id, Urho3D::IntVector2 bucketCords);
	ResourceEntity* load(dbload_resource_entities* resource) const;

private:
	ResourceEntity* create(int id, Urho3D::IntVector2 bucketCords, UId uid) const;
	unsigned currentUId;
};

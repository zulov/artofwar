#pragma once

struct UId;

namespace Urho3D {
	class IntVector2;
	class Vector2;
}

struct dbload_resource;
class ResourceEntity;

class ResourceFactory {
public:
	ResourceFactory() = default;
	~ResourceFactory() = default;

	ResourceEntity* create(int id, Urho3D::IntVector2 bucketCords);
	ResourceEntity* load(dbload_resource* resource) const;
	void setResUid(unsigned resUid);

private:
	ResourceEntity* create(int id, Urho3D::IntVector2 bucketCords, UId uid) const;
	unsigned currentUId;
};

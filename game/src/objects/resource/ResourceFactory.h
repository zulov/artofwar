#pragma once

struct UId;

namespace Urho3D {
	class UShortVector2;
	class ShortVector2;
	class Vector2;
}

struct dbload_resource;
class ResourceEntity;

class ResourceFactory {
public:
	ResourceFactory() = default;
	~ResourceFactory() = default;

	ResourceEntity* create(unsigned short id, Urho3D::UShortVector2 bucketCords);
	ResourceEntity* load(dbload_resource* resource) const;
	void setResUid(unsigned resUid);

private:
	ResourceEntity* create(unsigned short id, Urho3D::UShortVector2 bucketCords, UId uid) const;
	unsigned currentUId;
};

#pragma once

struct UId;

namespace Urho3D {
	class IntVector2;
	class Vector2;
}

struct dbload_building;
class Building;

class BuildingFactory {
public:
	BuildingFactory() = default;
	~BuildingFactory() = default;
	Building* create(int id, const Urho3D::IntVector2& bucketCords, int level, int player, UId uid) const;
	Building* load(dbload_building* building) const;
};

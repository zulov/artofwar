#pragma once

class Player;
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

	Building* create(int id, const Urho3D::IntVector2& bucketCords, char level, char player) const;
	Building* load(dbload_building* building) const;
private:
	Building* create(int id, const Urho3D::IntVector2& bucketCords, char level, Player* player,  unsigned uid) const;
};

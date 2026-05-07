#pragma once

class Player;
struct UId;

namespace Urho3D {
	class UShortVector2;
	class Vector2;
}

struct dbload_building;
class Building;

class BuildingFactory {
public:
	BuildingFactory() = default;
	~BuildingFactory() = default;

	Building* create(unsigned short id, const Urho3D::UShortVector2& bucketCords, unsigned char level,
					 unsigned char player) const;
	Building* load(dbload_building* building) const;
private:
	Building* create(unsigned short id, const Urho3D::UShortVector2& bucketCords, unsigned char level, Player* player,
					 unsigned uid) const;
};

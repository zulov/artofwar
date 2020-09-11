#pragma once

#include <vector>

namespace Urho3D
{
	class IntVector2;
	class Vector2;
}

struct dbload_building;
class Building;

class BuildingFactory {
public:
	BuildingFactory() = default;
	~BuildingFactory() = default;
	Building* create(int id, Urho3D::Vector2& center, int player, const Urho3D::IntVector2& _bucketCords, int level);
	Building* load(dbload_building* building);
};

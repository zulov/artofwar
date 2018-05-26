#pragma once

#include <vector>
#include "Building.h"


struct dbload_building;

class BuildingFactory {
public:
	BuildingFactory();
	~BuildingFactory();
	std::vector<Building*>* create(int id, Vector2& center, int player, const IntVector2& _bucketCords, int level);
	std::vector<Building*>* load(dbload_building* building);
private:
	std::vector<Building*>* buildings;
};


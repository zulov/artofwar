#pragma once

#include "Building.h"
#include <vector>


struct dbload_building;

class BuildingFactory {
public:
	BuildingFactory();
	~BuildingFactory();
	std::vector<Building*>* create(int id, Vector3* center, int player, IntVector2 _bucketCords, int level);
	std::vector<Building*>* load(dbload_building* building);
private:
	std::vector<Building*>* buildings;
};


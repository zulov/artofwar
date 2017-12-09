#pragma once
#include "objects/EntityFactory.h"
#include "Building.h"
#include <vector>


struct dbload_building;

class BuildingFactory :public EntityFactory {
public:
	BuildingFactory();
	~BuildingFactory();
	std::vector<Building*>* create(int id, Vector3* center, int player, IntVector2 _bucketCords);
	std::vector<Building*>* load(dbload_building* building);
private:
	std::vector<Building*>* buildings;
};


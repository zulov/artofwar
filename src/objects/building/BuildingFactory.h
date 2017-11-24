#pragma once
#include "objects/EntityFactory.h"
#include "Building.h"
#include <vector>
#include "objects/unit/Unit.h"


class BuildingFactory :public EntityFactory {
public:
	BuildingFactory();
	~BuildingFactory();
	std::vector<Building*>* create(int id, Vector3* center, int player, IntVector2 _bucketCords);
private:
	std::vector<Building*>* buildings;
};


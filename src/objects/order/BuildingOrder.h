#pragma once
#include "FutureOrder.h"

enum class BuildingActionType : char;
class Building;

class BuildingOrder : public FutureOrder {

	BuildingOrder(Building* building, BuildingActionType action, short id);
	BuildingOrder(std::vector<Physical*>* buildings, BuildingActionType action, short id);
private:
	std::vector<Building*> buildings;
};

#pragma once
#include "FutureOrder.h"

enum class BuildingActionType : char;
class Building;

class BuildingOrder : public FutureOrder {

	BuildingOrder(Building* building, BuildingActionType action, unsigned short id);
	BuildingOrder(std::vector<Physical*>* buildings, BuildingActionType action, unsigned short id);
};

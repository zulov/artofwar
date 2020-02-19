#pragma once
#include "FutureOrder.h"

enum class BuildingActionType : char;
class Building;

class BuildingOrder : public FutureOrder {
public:
	BuildingOrder(Building* building, BuildingActionType action, short id);
	BuildingOrder(std::vector<Physical*>* buildings, BuildingActionType action, short id);
	bool clean() override;
	bool expired() override;
private:
	std::vector<Building*> buildings;
};

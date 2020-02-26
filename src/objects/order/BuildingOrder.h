#pragma once
#include "FutureOrder.h"
#include <vector>
#include "objects/Physical.h"

enum class BuildingActionType : char;
class Building;

class BuildingOrder : public FutureOrder {
public:
	BuildingOrder(Building* building, BuildingActionType action, short id);
	BuildingOrder(std::vector<Physical*>* buildings, BuildingActionType action, short id);

	void clean() override;
	bool expired() override;

	void execute() override;
private:
	std::vector<Building*> buildings;
};

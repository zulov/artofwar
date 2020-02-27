#include "BuildingActionCommand.h"
#include "VectorUtils.h"

BuildingActionCommand::BuildingActionCommand(Building* building, BuildingActionType action, short id, char player)
	: AbstractCommand(player), action(action) {
	buildings.emplace_back(building);
}

BuildingActionCommand::BuildingActionCommand(std::vector<Physical*>* buildings, BuildingActionType action, short id,
                                             char player)
	: AbstractCommand(player), action(action) {
	for (auto building : *buildings) {
		//TODO performance spróbowaæ z insertem
		this->buildings.emplace_back(reinterpret_cast<Building*>(building));
	}
}

void BuildingActionCommand::clean() {
	cleanDead(buildings);
}

bool BuildingActionCommand::expired() {
	return buildings.empty();
}

#include "BuildingActionCommand.h"
#include "math/VectorUtils.h"
#include "objects/building/Building.h"

BuildingActionCommand::BuildingActionCommand(Building* building, BuildingActionType action, short id, char player)
	: AbstractCommand(player), id(id), action(action) {
	buildings.emplace_back(building);
}

BuildingActionCommand::BuildingActionCommand(std::vector<Physical*>* buildings, BuildingActionType action, short id,
                                             char player)
	: AbstractCommand(player), id(id), action(action) {
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

void BuildingActionCommand::execute() {
	for (auto building : buildings) {
		building->action(action, id);
	}
}

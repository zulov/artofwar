#include "BuildingActionCommand.h"
#include "VectorUtils.h"

BuildingActionCommand::BuildingActionCommand(Building* building, BuildingActionType action, short id, char player)
	: AbstractCommand(player), action(action), id(id) {
	buildings.emplace_back(building);
}

BuildingActionCommand::BuildingActionCommand(std::vector<Physical*>* buildings, BuildingActionType action, short id,
                                             char player)
	: AbstractCommand(player), action(action), id(id) {
	for (auto building : *buildings) {
		//TODO performance spr�bowa� z insertem
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
#include "BuildingActionCommand.h"
#include "math/VectorUtils.h"
#include "objects/building/Building.h"

BuildingActionCommand::BuildingActionCommand(Building* building, BuildingActionType action, short id, char player)
	: AbstractCommand(player), id(id), action(action) {
	buildings.emplace_back(building);
}

BuildingActionCommand::BuildingActionCommand(const std::vector<Physical*>& buildings, BuildingActionType action,
                                             short id,
                                             char player) : AbstractCommand(player), id(id), action(action) {
	this->buildings.reserve(buildings.size());
	for (auto building : buildings) {
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

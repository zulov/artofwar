#include "BuildingActionCommand.h"
#include "math/VectorUtils.h"
#include "objects/building/Building.h"

BuildingActionCommand::BuildingActionCommand(Building* building, BuildingActionType action, short id, char player)
	: AbstractCommand(player), id(id), action(action) {
	buildings.emplace_back(building);
	if (building->isReady()) {
		this->buildings.emplace_back(building);
	}
}

BuildingActionCommand::BuildingActionCommand(const std::vector<Physical*>& buildings, BuildingActionType action,
                                             short id,
                                             char player) : AbstractCommand(player), id(id), action(action) {
	this->buildings.reserve(buildings.size());
	for (auto building : buildings) {
		auto build = reinterpret_cast<Building*>(building);
		if (build->isReady()) {
			this->buildings.emplace_back(build);
		}
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

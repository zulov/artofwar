#include "BuildingActionCommand.h"
#include "math/VectorUtils.h"
#include "objects/building/Building.h"

BuildingActionCommand::BuildingActionCommand(Building* building, BuildingActionType action, short id)
	: id(id), action(action) {
	if (building->isReady()) {
		this->buildings.emplace_back(building);
	}
}

BuildingActionCommand::BuildingActionCommand(const std::vector<Physical*>& buildings, BuildingActionType action,
                                             short id) : id(id), action(action) {
	this->buildings.reserve(buildings.size());
	for (const auto building : buildings) {
		auto build = reinterpret_cast<Building*>(building);
		if (build->isReady()) {
			this->buildings.emplace_back(build);
		}
	}
}

void BuildingActionCommand::execute() {
	for (const auto building : buildings) {
		if (building->isAlive()) {
			building->action(action, id);
		}
	}
}

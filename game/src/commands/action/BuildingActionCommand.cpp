#include "BuildingActionCommand.h"
#include "math/VectorUtils.h"
#include "objects/building/Building.h"
#include "scene/load/RuntimeSaveData.h"

BuildingActionCommand::BuildingActionCommand(Building* building, BuildingActionType action, unsigned short id)
	: action(action), id(id) {
	if (building->isReady()) {
		this->buildings.emplace_back(building);
	}
}

BuildingActionCommand::BuildingActionCommand(const std::vector<Physical*>& buildings, BuildingActionType action,
                                             unsigned short id) : id(id), action(action) {
	this->buildings.reserve(buildings.size());
	for (const auto building : buildings) {
		auto build = static_cast<Building*>(building);
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

PendingCommandSaveData BuildingActionCommand::saveState(unsigned short order) const {
	PendingCommandSaveData state;
	state.order = order;
	state.kind = PendingCommandKind::BUILDING_ACTION;
	state.action = static_cast<char>(action);
	state.id = id;
	for (const auto* building : buildings) {
		state.entityUids.push_back(building->getUid());
	}
	return state;
}

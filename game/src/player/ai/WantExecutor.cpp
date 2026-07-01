#include "WantExecutor.h"

#include <limits>
#include <ranges>
#include <span>

#include "AiHistory.h"
#include "Game.h"
#include "MasterBrain.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/GeneralActionType.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "database/DatabaseCache.h"
#include "env/Environment.h"
#include "env/influence/CenterType.h"
#include "math/MathUtils.h"
#include "objects/building/Building.h"
#include "objects/building/ParentBuildingType.h"
#include "objects/queue/QueueActionType.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Possession.h"
#include "player/Resources.h"
#include "player/ai/ActionCenter.h"

WantExecutor::WantExecutor(Player* player, db_nation* nation, AiHistory* history) :
	player(player), playerId(player->getId()), possession(player->getPossession()),
	nation(nation), history(history), buildSpatialBrain(nation) {}

void WantExecutor::prepare(const MasterOutput& masterOut) {
	this->masterOut = &masterOut;
	pendingLackingBuilding = -1;
}

bool WantExecutor::execute(WantItem& item) {
	switch (item.type) {
	case WantItemType::WORKER:
		return executeWorker(item.specificId);
	case WantItemType::UNIT:
		return executeUnit(item.specificId);
	case WantItemType::BUILDING:
		return executeBuilding(item.specificId);
	case WantItemType::UNIT_UPGRADE:
		return executeUnitUpgrade(item.specificId);
	case WantItemType::BUILDING_UPGRADE:
		return executeBuildingUpgrade(item.specificId);
	}
	return false;
}

const db_with_cost* WantExecutor::cost(const WantItem& item) const {
	if (item.specificId < 0) { return nullptr; }

	switch (item.type) {
	case WantItemType::WORKER:
		return Game::getDatabase()->getUnit(item.specificId);
	case WantItemType::UNIT:
		return Game::getDatabase()->getUnit(item.specificId);
	case WantItemType::BUILDING:
		return Game::getDatabase()->getBuilding(item.specificId);
	case WantItemType::UNIT_UPGRADE:
		if (auto nextLevel = player->getNextLevelForUnit(item.specificId)) { return *nextLevel; }
		return nullptr;
	case WantItemType::BUILDING_UPGRADE:
		if (auto bNextLevel = player->getNextLevelForBuilding(item.specificId)) { return *bNextLevel; }
		return nullptr;
	}
	return nullptr;
}

// --- Execution ---

bool WantExecutor::executeWorker(short unitId) {
	if (unitId < 0) { return false; }
	auto* unit = Game::getDatabase()->getUnit(unitId);
	if (Building* building = pickDeployBuilding(unit, CenterType::ECON)) {
		Game::getActionCenter()->add(
				new BuildingActionCommand(building, BuildingActionType::UNIT_CREATE, unit->id));
		history->addAction(AiActionType::CREATE_WORKER, AiActionResult::SUCCESS);
		return true;
	}
	history->addAction(AiActionType::CREATE_WORKER, AiActionResult::NO_BUILDING_TO_DEPLOY);
	return false;
}

bool WantExecutor::executeUnit(short unitId) {
	if (unitId < 0) { return false; }
	auto* unit = Game::getDatabase()->getUnit(unitId);
	if (Building* building = pickDeployBuilding(unit, CenterType::ARMY)) {
		Game::getActionCenter()->add(
				new BuildingActionCommand(building, BuildingActionType::UNIT_CREATE, unit->id));
		history->addAction(AiActionType::CREATE_UNIT, AiActionResult::SUCCESS);
		return true;
	}
	pendingLackingBuilding = findBuildingTypeToDeploy(unitId);
	history->addAction(AiActionType::CREATE_UNIT, AiActionResult::NO_BUILDING_TO_DEPLOY);
	return false;
}

bool WantExecutor::executeBuilding(short buildingId) {
	if (buildingId < 0) { return false; }
	auto* building = Game::getDatabase()->getBuilding(buildingId);
	
	auto pos = findPosToBuild(building);
	if (pos.has_value()) {
		if (!Game::getActionCenter()->addBuilding(building->id, pos.value(), playerId, false)) {
			history->addAction(AiActionType::CREATE_BUILDING, AiActionResult::NO_POSITION_TO_BUILD);
			return false;
		}
		history->addAction(AiActionType::CREATE_BUILDING, AiActionResult::SUCCESS);
		return true;
	}
	history->addAction(AiActionType::CREATE_BUILDING, AiActionResult::NO_POSITION_TO_BUILD);
	return false;
}

bool WantExecutor::executeUnitUpgrade(short unitId) {
	if (unitId < 0) { return false; }
	auto nextLevel = player->getNextLevelForUnit(unitId);
	if (!nextLevel.has_value()) {
		history->addAction(AiActionType::UPGRADE_UNIT, AiActionResult::NO_UPGRADE_AVAILABLE);
		return false;
	}

	// Find a building that can handle this unit's upgrade
	auto buildings = getBuildingsCanDeploy(unitId);
	if (!buildings.empty()) {
		Game::getActionCenter()->add(
				new BuildingActionCommand(buildings[0], BuildingActionType::UNIT_LEVEL, unitId));
		history->addAction(AiActionType::UPGRADE_UNIT, AiActionResult::SUCCESS);
		return true;
	}

	// No building available — signal to build one
	pendingLackingBuilding = findBuildingTypeToDeploy(unitId);
	history->addAction(AiActionType::UPGRADE_UNIT, AiActionResult::NO_BUILDING_TO_DEPLOY);
	return false;
}

bool WantExecutor::executeBuildingUpgrade(short buildingId) {
	if (buildingId < 0) { return false; }
	auto nextLevel = player->getNextLevelForBuilding(buildingId);
	if (!nextLevel.has_value()) {
		history->addAction(AiActionType::UPGRADE_BUILDING, AiActionResult::NO_UPGRADE_AVAILABLE);
		return false;
	}
	Game::getActionCenter()->add(
			new GeneralActionCommand(buildingId, GeneralActionType::BUILDING_LEVEL, playerId));
	history->addAction(AiActionType::UPGRADE_BUILDING, AiActionResult::SUCCESS);
	return true;
}

// --- Deployment / placement helpers ---

Building* WantExecutor::pickDeployBuilding(db_unit* unit, CenterType center) const {
	std::vector<Building*> allPossible = getBuildingsCanDeploy(unit->id);
	if (allPossible.empty()) { return nullptr; }
	if (allPossible.size() == 1) { return allPossible[0]; }

	auto centerPos = Game::getEnvironment()->getCenterOf(center, playerId);
	if (!centerPos.has_value()) { return allPossible[0]; }

	auto target = centerPos.value();
	Building* best = allPossible[0];
	float bestDist = std::numeric_limits<float>::max();
	for (auto* b : allPossible) {
		float d = b->getPosition().SqDistXZ(target);
		if (d < bestDist) {
			bestDist = d;
			best = b;
		}
	}
	return best;
}

bool WantExecutor::buildingProducesUnit(db_building* building, unsigned short unitId) const {
	const auto* unitIds = player->getLevelForBuilding(building->id)->unitsPerNationIds[player->getNation()];
	return std::ranges::find(*unitIds, unitId) != unitIds->end();
}

std::vector<Building*> WantExecutor::getBuildingsCanDeploy(unsigned short unitId) const {
	std::vector<Building*> allPossible;
	allPossible.reserve(nation->buildings.size());
	for (const auto building : nation->buildings) {
		if (buildingProducesUnit(building, unitId)) {
			std::ranges::copy_if(*possession->getBuildings(building->id),
			                     std::back_inserter(allPossible), [](Building* b) { return b->isReady(); });
		}
	}
	return allPossible;
}

short WantExecutor::findBuildingTypeToDeploy(short unitId) const {
	for (const auto building : nation->buildings) {
		if (buildingProducesUnit(building, static_cast<unsigned short>(unitId))) { return building->id; }
	}
	return -1;
}

std::optional<Urho3D::Vector2> WantExecutor::findPosToBuild(db_building* building) {
	if (building->parentType[static_cast<unsigned char>(ParentBuildingType::RESOURCE)]) {
		return Game::getEnvironment()->getPosToCreateResBonus(building, playerId);
	}
	// Use BuildSpatialBrain to compute influence map weights

	auto spatialOut = buildSpatialBrain.decide(player, Game::getPlayersMan()->getEnemyFor(playerId),
			masterOut->buildingUrgency, masterOut->expandUrgency, masterOut->defenceBuildingUrgency
			);
	return Game::getEnvironment()->getPosToCreate(
			std::span<const float>(spatialOut.weights), type, building, playerId);
}

#include "ActionMaker.h"

#include <chrono>
#include <numeric>
#include <valarray>
#include "AiUtils.h"
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/GeneralActionType.h"
#include "database/DatabaseCache.h"
#include "math/VectorUtils.h"
#include "player/Player.h"
#include "player/ai/ActionCenter.h"
#include "simulation/env/Environment.h"
#include "stats/AiInputProvider.h"


ActionMaker::ActionMaker(Player* player): player(player),
                                          ifWorkersCreate("Data/ai/workersCreate_w.csv"),
                                          whereWorkersCreate(),

                                          ifBuildingCreate(),
                                          whatBuildingCreate(),
                                          whereBuildingCreate(),

                                          ifUnitCreate(),
                                          whatUnitCreate(),
                                          whereUnitCreate() {
}

const std::span<float> ActionMaker::decideFromBasic(Brain& brain) const {
	return brain.decide(Game::getAiInputProvider()->getBasicInput(player->getId()));
}

bool ActionMaker::createUnit(db_unit* unit) {
	if (enoughResources(unit)) {
		Building* building = getBuildingToDeploy(unit);
		if (building) {
			Game::getActionCenter()->add(
				new BuildingActionCommand(building, BuildingActionType::UNIT_CREATE, unit->id, player->getId()));
			return true;
		} else {
			//TODO try to build
		}
	}
	return false;
}

void ActionMaker::action() {
	auto resInput = Game::getAiInputProvider()->getResourceInput(player->getId());
	const auto resResult = ifWorkersCreate.decide(resInput);

	if (resResult[0] > 0.3f) {
		auto& units = Game::getDatabase()->getNation(player->getNation())->units;
		auto unit = units[4];
		createUnit(unit);
	}
	int a = 5;

	//return createUnit();
	//return createBuilding();
	//return levelUpUnit();
	//return levelUpBuilding();
}

bool ActionMaker::enoughResources(db_with_cost* withCosts) const {
	return withCosts && player->getResources().hasEnough(withCosts->costs);
}

bool ActionMaker::createBuilding() {
	const auto building = chooseBuilding();
	if (enoughResources(building)) {
		auto pos = posToBuild(building);
		if (pos.has_value()) {
			return Game::getActionCenter()->addBuilding(building->id, pos.value(), player->getId());
		}
	}
	return false;
}

bool ActionMaker::createUnit() {
	db_unit* unit = chooseUnit();
	return createUnit(unit);
}

db_building* ActionMaker::chooseBuilding() {
	auto& buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
	auto result = decideFromBasic(buildingBrainId);

	std::valarray<float> center(result.data(), result.size()); //TODO perf valarraay test
	std::vector<float> diffs;
	diffs.reserve(buildings.size());
	for (auto building : buildings) {
		diffs.push_back(dist(
			center, player->getLevelForBuilding(building->id)->dbBuildingMetricPerNation[player->getNation()]));
	}
	if (diffs.empty()) {
		return nullptr;
	}
	auto inx = lowestWithRand(diffs);

	const auto building = buildings[inx];
	logBuilding(building);
	return building;
}

db_building_level* ActionMaker::chooseBuildingLevelUp() {
	auto& buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
	auto result = decideFromBasic(buildingLevelUpId);

	// std::valarray<float> center(result.data(), result.size()); //TODO perf valarraay test
	// std::vector<float> diffs;
	// diffs.reserve(buildings.size());
	// for (auto building : buildings) {
	// 	auto opt = player->getNextLevelForBuilding(building->id);
	// 	if (opt.has_value()) {
	// 		diffs.push_back(dist(center, opt.value()->dbBuildingMetricUp));
	// 	} else {
	// 		diffs.push_back(1000);
	// 	}
	// }
	// if (diffs.empty() ) {
	// 	return nullptr;
	// }
	// auto inx = lowestWithRand(diffs);
	//
	// const auto building = buildings[inx];
	// auto opt = player->getNextLevelForBuilding(building->id);
	// if (opt.has_value()) {
	// 	logBuildingLevel(building, opt);
	// 	return opt.value();
	// }
	return nullptr;
}

db_unit* ActionMaker::chooseUnit() {
	auto& units = Game::getDatabase()->getNation(player->getNation())->units;
	auto result = decideFromBasic(unitBrainId);

	std::valarray<float> center(result.data(), result.size()); //TODO perf valarraay test
	std::vector<float> diffs;
	diffs.reserve(units.size());
	for (auto unit : units) {
		diffs.push_back(dist(center, player->getLevelForUnit(unit->id)->dbUnitMetric));
	}
	if (diffs.empty()) {
		return nullptr;
	}
	auto inx = lowestWithRand(diffs);

	auto unit = units[inx];

	logUnit(unit);
	return unit;
}

db_unit_level* ActionMaker::chooseUnitLevelUp() {
	auto& units = Game::getDatabase()->getNation(player->getNation())->units;
	auto result = decideFromBasic(unitLevelUpId);

	// std::valarray<float> center(result.data(), result.size()); //TODO perf valarraay test
	// std::vector<float> diffs;
	// diffs.reserve(units.size());
	// for (auto unit : units) {
	// 	auto opt = player->getNextLevelForUnit(unit->id);
	// 	if (opt.has_value()) {
	// 		diffs.push_back(dist(center, opt.value()->dbUnitMetricUp));
	// 	} else {
	// 		diffs.push_back(1000);
	// 	}
	// }
	// if (diffs.empty() ) {
	// 	return nullptr;
	// }
	// auto inx = lowestWithRand(diffs);
	//
	// auto unit = units[inx];
	// auto opt = player->getNextLevelForUnit(unit->id);
	// if (opt.has_value()) {
	// 	logUnitLevel(unit, opt);
	// 	return opt.value();
	// }
	return nullptr;
}

float ActionMaker::dist(std::valarray<float>& center, const db_basic_metric* metric) {
	auto span = metric->getParamsAsSpan();
	std::valarray<float> aiAsArray(span.data(), span.size()); //TODO get as val array odrazu
	auto diff = aiAsArray - center;
	auto sq = diff * diff;
	return sq.sum();
}

const std::span<float> ActionMaker::inputWithParamsDecide(Brain& brain, const db_basic_metric* metric) const {
	return brain.decide(Game::getAiInputProvider()->getBasicInputWithMetric(player->getId(), metric));
}

std::optional<Urho3D::Vector2> ActionMaker::posToBuild(db_building* building) {
	auto result = inputWithParamsDecide(buildingBrainPos,
	                                    player->getLevelForBuilding(building->id)
	                                          ->dbBuildingMetricPerNation[player->getNation()]);

	return Game::getEnvironment()->getPosToCreate(building, player->getId(), result);
}

std::vector<Building*> ActionMaker::getBuildingsCanDeploy(short unitId, std::vector<db_building*>& buildings) const {
	std::vector<short> buildingIdsThatCanDeploy;
	for (auto building : buildings) {
		auto unitIds = player->getLevelForBuilding(building->id)->unitsPerNationIds[player->getNation()];

		if (std::find(unitIds->begin(), unitIds->end(), unitId) != unitIds->end()) {
			buildingIdsThatCanDeploy.push_back(building->id);
		}
	}
	std::vector<Building*> allPossible;
	for (auto thatCanDeploy : buildingIdsThatCanDeploy) {
		auto buildingEnts = player->getPossession().getBuildings(thatCanDeploy);
		allPossible.insert(allPossible.end(), buildingEnts->begin(), buildingEnts->end());
	}
	return allPossible;
}

Building* ActionMaker::getBuildingToDeploy(db_unit* unit) {
	auto& buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
	std::vector<Building*> allPossible = getBuildingsCanDeploy(unit->id, buildings);
	if (allPossible.empty()) { return nullptr; }

	auto result = inputWithParamsDecide(unitBrainPos, player->getLevelForUnit(unit->id)->dbUnitMetric);
	//TODO improve last parameter ignored queue size
	auto centers = Game::getEnvironment()->getAreas(player->getId(), result, 10);
	float closestVal = 99999;
	Building* closest = allPossible[0];
	for (auto possible : allPossible) {
		//TODO performance O(^2)
		Urho3D::Vector2 pos = {possible->getPosition().x_, possible->getPosition().z_};
		for (auto& center : centers) {
			auto diff = pos - center;

			auto val = diff.LengthSquared();
			if (val < closestVal) {
				closest = possible;
				closestVal = val;
			}
		}
	}

	return closest;
}

Building* ActionMaker::getBuildingToLevelUpUnit(db_unit_level* level) {
	auto& buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
	std::vector<Building*> allPossible = getBuildingsCanDeploy(level->unit, buildings);
	// if (allPossible.empty()) { return nullptr; }
	// auto& result = inputWithParamsDecide(unitLevelUpPos, level->dbUnitMetricUp);
	// float val = result[0] * 10.f; //TODO hard code DEFAULT_NORMALIZE_VALUE
	// float closestVal = 99999;
	// Building* closest = allPossible[0];
	// for (auto possible : allPossible) {
	// 	auto diff = val - possible->getQueue()->getSize();
	// 	diff = diff * diff;
	// 	if (diff < closestVal) {
	// 		closest = possible;
	// 		closestVal = diff;
	// 	}
	// }
	// return closest;
	return nullptr;
}

bool ActionMaker::levelUpBuilding() {
	const auto level = chooseBuildingLevelUp();
	if (enoughResources(level)) {
		Game::getActionCenter()->add(
			new GeneralActionCommand(level->building, GeneralActionType::BUILDING_LEVEL, player->getId()));
		return true;
	}
	return false;
}

bool ActionMaker::levelUpUnit() {
	db_unit_level* level = chooseUnitLevelUp();
	if (enoughResources(level)) {
		Building* building = getBuildingToLevelUpUnit(level);
		if (building) {
			Game::getActionCenter()->add(
				new BuildingActionCommand(building, BuildingActionType::UNIT_LEVEL, level->unit, player->getId()));
			return true;
		}
	}
	return false;
}

#include "ActionMaker.h"

#include "AiUtils.h"
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/GeneralActionType.h"
#include "database/DatabaseCache.h"
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "objects/building/Building.h"
#include "player/Player.h"
#include "player/ai/ActionCenter.h"
#include "simulation/env/Environment.h"
#include "stats/AiInputProvider.h"


ActionMaker::ActionMaker(Player* player): player(player),
                                          ifWorkerCreate(BrainProvider::get("ifWorkerCreate_w.csv")),
                                          whereWorkerCreate(BrainProvider::get("whereWorkerCreate_w.csv")),

                                          ifBuildingCreate(BrainProvider::get("ifBuildingCreate_w.csv")),
                                          whichBuildingCreate(BrainProvider::get("whichBuildingCreate_w.csv")),
                                          whereBuildingCreate(BrainProvider::get("whereBuildingCreate_w.csv")),

                                          ifUnitCreate(BrainProvider::get("ifUnitCreate_w.csv")),
                                          whichUnitCreate(BrainProvider::get("whichUnitCreate_w.csv")),
                                          whereUnitCreate(BrainProvider::get("whereUnitCreate_w.csv")) {
}

void ActionMaker::action() {
	auto aiInput = Game::getAiInputProvider();
	const auto resInput = aiInput->getResourceInput(player->getId());
	const auto unitsInput = aiInput->getUnitsInput(player->getId()); //TODO czy cokolwiek?
	const auto buildingsInput = aiInput->getBuildingsInput(player->getId());

	const auto resResult = ifWorkerCreate->decide(resInput);
	if (resResult[0] > 0.5f) {
		auto& units = Game::getDatabase()->getNation(player->getNation())->units;
		auto unit = units[4]; //TODO lepiej wybrac
		auto result = createWorker(unit);
	}

	const auto unitsResult = ifUnitCreate->decide(unitsInput);
	if (unitsResult[0] > 0.5f) {
		auto whichOutput = whichUnitCreate->decide(unitsInput);
		auto unit = chooseUnit(whichOutput);

		auto result = createUnit(unit);
	}

	const auto buildingsResult = ifBuildingCreate->decide(buildingsInput);
	if (buildingsResult[0] > 0.1f) {
		auto whichOutput = whichBuildingCreate->decide(buildingsInput);
		auto building = chooseBuilding(whichOutput);
		auto result = createBuilding(building);
	}

	//return levelUpUnit();
	//return levelUpBuilding();
}

bool ActionMaker::createUnit(db_unit* unit, Building* building) const {
	if (building) {
		Game::getActionCenter()->add(
			new BuildingActionCommand(building, BuildingActionType::UNIT_CREATE, unit->id, player->getId()));
		return true;
	} else {
		//TODO try to build
		return false;
	}
}

bool ActionMaker::createWorker(db_unit* unit) {
	if (enoughResources(unit)) {
		Building* building = getBuildingToDeployWorker(unit);
		return createUnit(unit, building);
	}
	return false;
}

bool ActionMaker::createUnit(db_unit* unit) {
	if (enoughResources(unit)) {
		Building* building = getBuildingToDeploy(unit);
		return createUnit(unit, building);
	}
	return false;
}

bool ActionMaker::enoughResources(db_with_cost* withCosts) const {
	return withCosts && player->getResources().hasEnough(withCosts->costs);
}

bool ActionMaker::createBuilding(db_building* building) {
	if (enoughResources(building)) {
		auto pos = posToBuild(building);
		if (pos.has_value()) {
			return Game::getActionCenter()->addBuilding(building->id, pos.value(), player->getId());
		}
	}
	return false;
}

db_building* ActionMaker::chooseBuilding(std::span<float> result) {
	auto& buildings = Game::getDatabase()->getNation(player->getNation())->buildings;

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
	//auto result = decideFromBasic(buildingLevelUpId);

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

db_unit* ActionMaker::chooseUnit(std::span<float> result) {
	auto& units = Game::getDatabase()->getNation(player->getNation())->units;
	std::vector<db_unit*> unitsWithoutWorker(units.size());
	auto pred = [this](db_unit* unit) { return !player->getLevelForUnit(unit->id)->canCollect; };
	auto it = std::copy_if(units.begin(), units.end(), unitsWithoutWorker.begin(), pred);
	unitsWithoutWorker.resize(std::distance(unitsWithoutWorker.begin(), it));

	std::valarray<float> center(result.data(), result.size()); //TODO perf valarraay test
	std::vector<float> diffs;
	diffs.reserve(unitsWithoutWorker.size());
	for (auto unit : unitsWithoutWorker) {
		diffs.push_back(dist(center, player->getLevelForUnit(unit->id)->dbUnitMetric));
	}
	if (diffs.empty()) {
		return nullptr;
	}
	const auto inx = lowestWithRand(diffs);

	const auto unit = unitsWithoutWorker[inx];

	logUnit(unit);
	return unit;
}

db_unit_level* ActionMaker::chooseUnitLevelUp() {
	auto& units = Game::getDatabase()->getNation(player->getNation())->units;
	//auto result = decideFromBasic(unitLevelUpId);

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

std::optional<Urho3D::Vector2> ActionMaker::posToBuild(db_building* building) {
	const auto input = Game::getAiInputProvider()->getBuildingsInputWithMetric(
		player->getId(), player->getLevelForBuilding(building->id)
		                       ->dbBuildingMetricPerNation[player->getNation()]);
	const auto result = whereBuildingCreate->decide(input);

	return Game::getEnvironment()->getPosToCreate(building, player->getId(), result);
}

std::vector<Building*> ActionMaker::getBuildingsCanDeploy(short unitId) const {
	auto& buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
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
	std::vector<Building*> allPossible = getBuildingsCanDeploy(unit->id);
	if (allPossible.empty()) { return nullptr; }
	const auto input = Game::getAiInputProvider()
		->getUnitsInputWithMetric(player->getId(),
		                          player->getLevelForUnit(unit->id)->dbUnitMetric);
	const auto result = whereUnitCreate->decide(input);

	return getBuildingClosestArea(allPossible, result);
}

Building* ActionMaker::getBuildingToDeployWorker(db_unit* unit) {
	std::vector<Building*> allPossible = getBuildingsCanDeploy(unit->id);
	if (allPossible.empty()) { return nullptr; }
	const auto input = Game::getAiInputProvider()->getResourceInput(player->getId());

	const auto result = whereWorkerCreate->decide(input);

	return getBuildingClosestArea(allPossible, result);
}

Building* ActionMaker::getBuildingClosestArea(std::vector<Building*>& allPossible, std::span<float> result) const {
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
	//std::vector<Building*> allPossible = getBuildingsCanDeploy(level->unit, buildings);
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

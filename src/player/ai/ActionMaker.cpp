#include "ActionMaker.h"

#include <format>
#include "AiTypes.h"
#include "AiUtils.h"
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/GeneralActionType.h"
#include "nn/Brain.h"
#include "nn/BrainProvider.h"
#include "objects/building/Building.h"
#include "player/Player.h"
#include "player/ai/ActionCenter.h"
#include "env/Environment.h"
#include "stats/AiInputProvider.h"


ActionMaker::ActionMaker(Player* player, db_nation* nation):
	player(player), nation(nation),
	ifWorkerCreate(BrainProvider::get(nation->actionPrefix[0] + "ifWorkerCreate.csv")),
	whereWorkerCreate(BrainProvider::get(nation->actionPrefix[1] + "whereWorkerCreate.csv")),

	ifBuildingCreate(BrainProvider::get(nation->actionPrefix[2] + "ifBuildingCreate.csv")),
	whichBuildingCreate(BrainProvider::get(nation->actionPrefix[3] + "whichBuildingCreate.csv")),
	whereBuildingCreate(BrainProvider::get(nation->actionPrefix[4] + "whereBuildingCreate.csv")),

	ifUnitCreate(BrainProvider::get(nation->actionPrefix[5] + "ifUnitCreate.csv")),
	whichUnitCreate(BrainProvider::get(nation->actionPrefix[6] + "whichUnitCreate.csv")),
	whereUnitCreate(BrainProvider::get(nation->actionPrefix[7] + "whereUnitCreate.csv")) {

	// std::cout << std::format("AI Sizes OUTPUT\t Res: {}, Unit: {}, Build: {}, UnitM: {}, BuildM: {}\n",
	//                          ifWorkerCreate->getOutputSize(), ifUnitCreate->getOutputSize(),
	//                          ifBuildingCreate->getOutputSize(), whereUnitCreate->getOutputSize(),
	//                          whereBuildingCreate->getOutputSize());
}

bool ActionMaker::createBuilding(const std::span<float> buildingsInput) {
	const auto whichOutput = whichBuildingCreate->decide(buildingsInput);
	const auto building = chooseBuilding(whichOutput);
	return createBuilding(building);
}

bool ActionMaker::createWorker() {
	for (const auto dbUnit : nation->units) {
		if (dbUnit->typeWorker) {
			return createWorker(dbUnit);
		}
	}
	assert(false);
	return false;
}

bool ActionMaker::createUnit(std::span<float> unitsInput) {
	const auto whichOutput = whichUnitCreate->decide(unitsInput);
	const auto unit = chooseUnit(whichOutput);

	return createUnit(unit);
}

bool ActionMaker::execute(const std::span<float> unitsInput, const std::span<float> buildingsInput,
                          AiActionType decision) {
	switch (decision) {
	case AiActionType::WORKER_CREATE:
		return createWorker();
	case AiActionType::UNIT_CREATE:
		return createUnit(unitsInput);
	case AiActionType::BUILDING_CREATE:
		return createBuilding(buildingsInput);
	default: ;
	}
	return false;
}

void ActionMaker::action() {
	auto aiInput = Game::getAiInputProvider();
	const auto resInput = aiInput->getResourceInput(player->getId());
	const auto unitsInput = aiInput->getUnitsInput(player->getId()); //TODO czy cokolwiek?
	const auto buildingsInput = aiInput->getBuildingsInput(player->getId());

	const auto resResult = ifWorkerCreate->decide(resInput);
	const auto unitsResult = ifUnitCreate->decide(unitsInput);
	const auto buildingsResult = ifBuildingCreate->decide(buildingsInput);

	float res[] = {resResult[0], unitsResult[0], buildingsResult[0]};

	auto decision = static_cast<AiActionType>(biggestWithRand(std::span(res)));
	execute(unitsInput, buildingsInput, decision);

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

bool ActionMaker::createWorker(db_unit* unit) const {
	if (enoughResources(unit)) {
		Building* building = getBuildingToDeployWorker(unit);
		return createUnit(unit, building);
	}
	return false;
}

bool ActionMaker::createUnit(db_unit* unit) const {
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
		auto pos = findPosToBuild(building);
		if (pos.has_value()) {
			return Game::getActionCenter()->addBuilding(building->id, pos.value(), player->getId(), false);
		}
	}
	return false;
}

db_building* ActionMaker::chooseBuilding(std::span<float> result) {
	const auto& buildings = nation->buildings;

	std::valarray<float> center(result.data(), result.size()); //TODO perf valarraay test
	std::vector<float> diffs;
	diffs.reserve(buildings.size());
	for (const auto building : buildings) {
		diffs.push_back(dist(center, player->getLevelForBuilding(building->id)->dbBuildingMetric));
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
	auto& buildings = nation->buildings;
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
	auto pred = [this](db_unit* unit) { return !unit->typeWorker; };
	auto& units = nation->units;
	std::vector<db_unit*> unitsWithoutWorker;
	unitsWithoutWorker.reserve(units.size());
	std::copy_if(units.begin(), units.end(), std::back_inserter(unitsWithoutWorker), pred);

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
	auto& units = nation->units;
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
	auto& aiAsArray = metric->getValuesNormAsVal();
	auto diff = aiAsArray - center;
	diff *= diff;
	return diff.sum();
}

std::optional<Urho3D::Vector2> ActionMaker::findPosToBuild(db_building* building) const {
	const auto input = Game::getAiInputProvider()->getBuildingsInputWithMetric(
		player->getId(), player->getLevelForBuilding(building->id)->dbBuildingMetric);
	const auto result = whereBuildingCreate->decide(input);

	return Game::getEnvironment()->getPosToCreate(building, player->getId(), result);
}

std::vector<Building*> ActionMaker::getBuildingsCanDeploy(short unitId) const {
	const auto& buildings = nation->buildings;
	std::vector<short> buildingIdsThatCanDeploy;
	for (const auto building : buildings) {
		auto unitIds = player->getLevelForBuilding(building->id)->unitsPerNationIds[player->getNation()];

		if (std::ranges::find(*unitIds, unitId) != unitIds->end()) {
			buildingIdsThatCanDeploy.push_back(building->id);
		}
	}
	std::vector<Building*> allPossible;
	for (auto thatCanDeploy : buildingIdsThatCanDeploy) {
		std::ranges::copy_if(*player->getPossession().getBuildings(thatCanDeploy),
		                     std::back_inserter(allPossible), [](Building* b) { return b->isReady(); });
	}
	return allPossible;
}

Building* ActionMaker::getBuildingToDeploy(db_unit* unit) const {
	std::vector<Building*> allPossible = getBuildingsCanDeploy(unit->id);
	if (allPossible.empty()) { return nullptr; }
	if (allPossible.size() == 1) { return allPossible.at(0); }
	const auto input = Game::getAiInputProvider()
		->getUnitsInputWithMetric(player->getId(), player->getLevelForUnit(unit->id)->dbUnitMetric);
	const auto result = whereUnitCreate->decide(input);

	return getBuildingClosestArea(allPossible, result);
}

Building* ActionMaker::getBuildingToDeployWorker(db_unit* unit) const {
	std::vector<Building*> allPossible = getBuildingsCanDeploy(unit->id);
	if (allPossible.empty()) { return nullptr; }
	if (allPossible.size() == 1) { return allPossible.at(0); }
	const auto input = Game::getAiInputProvider()->getResourceInput(player->getId());

	const auto result = whereWorkerCreate->decide(input);

	return getBuildingClosestArea(allPossible, result);
}

Building* ActionMaker::getBuildingClosestArea(std::vector<Building*>& allPossible, std::span<float> result) const {
	auto centers = Game::getEnvironment()->getAreas(player->getId(), result, 10);
	float closestVal = 99999;
	Building* closest = allPossible[0];
	for (const auto possible : allPossible) {
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
	auto& buildings = nation->buildings;
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

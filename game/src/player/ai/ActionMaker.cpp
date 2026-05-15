#include "ActionMaker.h"

#include <format>
#include <limits>
#include "AiHistory.h"
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
#include "math/MathUtils.h"
#include "objects/building/ParentBuildingType.h"
#include "player/PlayersManager.h"
#include "player/Possession.h"
#include "player/Resources.h"
#include "stats/AiInputProvider.h"


ActionMaker::ActionMaker(Player* player, db_nation* nation, AiHistory* history) :
	player(player), playerId(player->getId()), possession(player->getPossession()), nation(nation),
	ifWorker(BrainProvider::get(nation->actionPrefix[0] + "ifWorker.csv")),
	whereWorker(BrainProvider::get(nation->actionPrefix[1] + "whereWorker.csv")),

	ifBuilding(BrainProvider::get(nation->actionPrefix[2] + "ifBuilding.csv")),
	whichBuildingType(BrainProvider::get(nation->actionPrefix[3] + "whichBuildingType.csv")),
	whichBuildingTypeOther(BrainProvider::get(nation->actionPrefix[4] + "whichBuildTypeOther.csv")),
	whichBuildingTypeDefence(BrainProvider::get(nation->actionPrefix[5] + "whichBuildTypeDefence.csv")),
	whichBuildingTypeResource(BrainProvider::get(nation->actionPrefix[6] + "whichBuildTypeResource.csv")),
	whichBuildingTypeTech(BrainProvider::get(nation->actionPrefix[7] + "whichBuildTypeTech.csv")),
	whichBuildingTypeUnits(BrainProvider::get(nation->actionPrefix[8] + "whichBuildTypeUnits.csv")),
	whereBuilding(BrainProvider::get(nation->actionPrefix[9] + "whereBuilding.csv")),
	ifFarmBuild(BrainProvider::get(nation->actionPrefix[10] + "ifFarmBuild.csv")),

	ifUnit(BrainProvider::get(nation->actionPrefix[11] + "ifUnit.csv")),
	whichUnit(BrainProvider::get(nation->actionPrefix[12] + "whichUnit.csv")),
	whereUnit(BrainProvider::get(nation->actionPrefix[13] + "whereUnit.csv")),
	aiInput(Game::getAiInputProvider()),
	history(history) {
}


void ActionMaker::action() {
	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);
	if (isEnoughResToWorker()) {
		if (ifWorker->decide(aiInput->forResource(player, enemy)).doIf()) {
			createWorker();
		}
	}

	if (isEnoughResToAnyUnit()) {
		const auto unitsInput = aiInput->forUnits(player);
		if (ifUnit->decide(unitsInput).doIf()) {
			createUnit(unitsInput);
		}
	}
	if (isEnoughResToAnyBuilding()) {
		const auto buildingsInput = aiInput->forBuildings(player);
		if (ifBuilding->decide(buildingsInput).doIf()) {
			createBuilding(buildingsInput);
		}
	}

	//return levelUpUnit();
	//return levelUpBuilding();
}

namespace {
	AiActionType toBuildingActionType(ParentBuildingType type) {
		switch (type) {
		case ParentBuildingType::OTHER: return AiActionType::CREATE_BUILDING_OTHER;
		case ParentBuildingType::DEFENCE: return AiActionType::CREATE_BUILDING_DEFENCE;
		case ParentBuildingType::RESOURCE: return AiActionType::CREATE_BUILDING_RESOURCE;
		case ParentBuildingType::TECH: return AiActionType::CREATE_BUILDING_TECH;
		case ParentBuildingType::UNITS: return AiActionType::CREATE_BUILDING_UNITS;
		}
		return AiActionType::NONE;
	}
}

void ActionMaker::createBuilding(const std::span<const float> buildingsInput) {
	ParentBuildingType type = static_cast<ParentBuildingType>(whichBuildingType->decide(buildingsInput).best());
	AiActionType actionType = toBuildingActionType(type);
	if (!isEnoughResToTypeBuilding(type)) {
		history->addAction(actionType, AiActionResult::NO_RESOURCES_SPECIFIC);
		return;
	}
	if (type == ParentBuildingType::RESOURCE) {
		if (sumSpan(possession->getResWithOutBonus()) < 0.5f) {
			history->addAction(actionType, AiActionResult::NO_RESOURCE_BONUS);
			return;
		}
	}

	auto* building = chooseBuilding(type);
	if (!building) {
		history->addAction(actionType, AiActionResult::NO_BUILDINGS_OF_TYPE);
		return;
	}
	createBuilding(actionType, building, type);
}

void ActionMaker::createWorker() {
	assert(!nation->workers.empty());

	createWorker(AiActionType::CREATE_WORKER, nation->workers.at(0)); //TODO get better
}

void ActionMaker::createUnit(std::span<const float> unitsInput) {
	const auto whichOutput = whichUnit->decide(unitsInput);
	const auto unit = chooseUnit(whichOutput);

	createUnit(AiActionType::CREATE_UNIT, unit);
}

void ActionMaker::createUnit(AiActionType actionType, db_unit* unit, Building* building) {
	if (building) {
		Game::getActionCenter()->add(
			new BuildingActionCommand(building, BuildingActionType::UNIT_CREATE, unit->id));
		history->addAction(actionType, AiActionResult::SUCCESS);
		return;
	}
	//TODO try to build
	history->addAction(actionType, AiActionResult::NO_BUILDING_TO_DEPLOY);
}

void ActionMaker::createWorker(AiActionType actionType, db_unit* unit) {
	if (enoughResources(unit, player)) {
		Building* building = getBuildingToDeployWorker(unit);
		createUnit(actionType, unit, building);
		return;
	}
	history->addAction(actionType, AiActionResult::NO_RESOURCES_SPECIFIC);
}

void ActionMaker::createUnit(AiActionType actionType, db_unit* unit) {
	if (enoughResources(unit, player)) {
		Building* building = getBuildingToDeploy(unit);
		createUnit(actionType, unit, building);
		return;
	}
	history->addAction(actionType, AiActionResult::NO_RESOURCES_SPECIFIC);
}

bool ActionMaker::enoughResources(const db_with_cost* withCosts, Player* player) const {
	return withCosts && player->getResources()->hasEnough(withCosts);
}

void ActionMaker::createBuilding(AiActionType actionType, db_building* building, ParentBuildingType type) {
	if (enoughResources(building, player)) {
		auto pos = findPosToBuild(building, type);
		if (pos.has_value()) {
			Game::getActionCenter()->addBuilding(building->id, pos.value(), playerId, false);
			history->addAction(actionType, AiActionResult::SUCCESS);
			return;
		}
		history->addAction(actionType, AiActionResult::NO_POSITION_TO_BUILD);
		return;
	}
	history->addAction(actionType, AiActionResult::NO_RESOURCES_SPECIFIC);
}

std::vector<db_building*> ActionMaker::getBuildingsInType(ParentBuildingType type) {
	std::vector<db_building*> buildings;
	for (auto dbBuilding : nation->buildings) {
		if (dbBuilding->parentType[castC(type)]) {
			buildings.push_back(dbBuilding);
		}
	}
	return buildings;
}

Brain* ActionMaker::getBrainForBuildingType(ParentBuildingType type) const {
	switch (type) {
	case ParentBuildingType::OTHER:    return whichBuildingTypeOther;
	case ParentBuildingType::DEFENCE:  return whichBuildingTypeDefence;
	case ParentBuildingType::RESOURCE: return whichBuildingTypeResource;
	case ParentBuildingType::TECH:     return whichBuildingTypeTech;
	case ParentBuildingType::UNITS:    return whichBuildingTypeUnits;
	}
	assert(false && "Unknown ParentBuildingType");
	return nullptr;
}

db_building* ActionMaker::chooseBuilding(ParentBuildingType type) {
	const auto buildings = getBuildingsInType(type);
	if (buildings.empty()) {
		return nullptr;
	}
	if (buildings.size() == 1) {
		return buildings.at(0);
	}

	const auto result = getBrainForBuildingType(type)->decide(aiInput->forBuildingType(player, type));

	if (type == ParentBuildingType::RESOURCE) {
		const int res = result.best();
		for (const auto building : buildings) {
			if (building->resourceType == res) { return building; }
		}
		return nullptr;
	}

	std::valarray<float> center(result.data.data(), result.size());
	std::vector<float> diffs;
	diffs.reserve(buildings.size());
	for (const auto building : buildings) {
		diffs.push_back(dist(center, player->getLevelForBuilding(building->id)->dbBuildingMetric, type));
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
	// 	auto opt = player->getNextLevelForBuilding(building->dbId);
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
	// auto opt = player->getNextLevelForBuilding(building->dbId);
	// if (opt.has_value()) {
	// 	logBuildingLevel(building, opt);
	// 	return opt.value();
	// }
	return nullptr;
}

db_unit* ActionMaker::chooseUnit(const DecideResult& result) {
	auto pred = [this](db_unit* unit) { return !unit->typeWorker; };
	auto& units = nation->units;
	std::vector<db_unit*> unitsWithoutWorker;
	unitsWithoutWorker.reserve(units.size());
	std::ranges::copy_if(units, std::back_inserter(unitsWithoutWorker), pred);

	std::valarray<float> center(result.data.data(), result.size()); //TODO perf valarraay test
	std::vector<float> diffs;
	diffs.reserve(unitsWithoutWorker.size());
	for (const auto unit : unitsWithoutWorker) {
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
	// 	auto opt = player->getNextLevelForUnit(unit->dbId);
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
	// auto opt = player->getNextLevelForUnit(unit->dbId);
	// if (opt.has_value()) {
	// 	logUnitLevel(unit, opt);
	// 	return opt.value();
	// }
	return nullptr;
}

float ActionMaker::dist(std::valarray<float>& center, const db_basic_metric* metric) {
	return sqRootSumError(center, metric->getValuesNormAsVal());
}

float ActionMaker::dist(std::valarray<float>& center, const db_building_metric* metric, ParentBuildingType type) {
	return sqRootSumError(center, metric->getValuesNormAsValForType(type));
}

std::optional<Urho3D::Vector2> ActionMaker::findPosToBuild(db_building* building, ParentBuildingType type) const {
	if (type == ParentBuildingType::RESOURCE) {
		return Game::getEnvironment()->getPosToCreateResBonus(building, playerId);
	}
	const auto input = aiInput->forBuildingPlacement(player,
	                                                        player->getLevelForBuilding(building->id)->dbBuildingMetric,
	                                                        type);

	return Game::getEnvironment()->getPosToCreate(whereBuilding->decide(input), type, building, playerId);
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
		std::ranges::copy_if(*possession->getBuildings(thatCanDeploy),
		                     std::back_inserter(allPossible), [](Building* b) { return b->isReady(); });
	}
	return allPossible;
}

Building* ActionMaker::getBuildingToDeploy(db_unit* unit) const {
	std::vector<Building*> allPossible = getBuildingsCanDeploy(unit->id);
	if (allPossible.empty()) { return nullptr; }
	if (allPossible.size() == 1) { return allPossible.at(0); }
	const auto input = aiInput->forUnitPlacement(player, player->getLevelForUnit(unit->id)->dbUnitMetric);
	const auto result = whereUnit->decide(input);

	return getBuildingClosestArea(allPossible, result);
}

Building* ActionMaker::getBuildingToDeployWorker(db_unit* unit) const {
	std::vector<Building*> allPossible = getBuildingsCanDeploy(unit->id);
	if (allPossible.empty()) { return nullptr; }
	if (allPossible.size() == 1) { return allPossible.at(0); }
	const auto enemy = Game::getPlayersMan()->getEnemyFor(playerId);

	const auto result = whereWorker->decide(aiInput->forResource(player, enemy));

	return getBuildingClosestArea(allPossible, result);
}

Building* ActionMaker::getBuildingClosestArea(std::vector<Building*>& allPossible, const DecideResult& result) const {
	auto centers = Game::getEnvironment()->getAreas(playerId, result, 10);
	float closestVal = std::numeric_limits<float>::max();
	Building* closest = allPossible[0];
	for (const auto possible : allPossible) {
		//TODO performance O(^2)
		const Urho3D::Vector2 &pos = possible->getPosition();
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
	if (enoughResources(level, player)) {
		Game::getActionCenter()->add(
			new GeneralActionCommand(level->building, GeneralActionType::BUILDING_LEVEL, playerId));
		return true;
	}
	return false;
}

bool ActionMaker::levelUpUnit() {
	db_unit_level* level = chooseUnitLevelUp();
	if (enoughResources(level, player)) {
		Building* building = getBuildingToLevelUpUnit(level);
		if (building) {
			Game::getActionCenter()->add(
				new BuildingActionCommand(building, BuildingActionType::UNIT_LEVEL, level->unit));
			return true;
		}
	}
	return false;
}


bool ActionMaker::isEnoughResToWorker() const {
	return std::ranges::any_of(nation->workers, [this](const db_with_cost* thing) {
		return enoughResources(thing, player);
	});
}

bool ActionMaker::isEnoughResToAnyUnit() const {
	return std::ranges::any_of(nation->units, [this](const db_unit* thing) {
		return !thing->typeWorker && enoughResources(thing, player);
	});
}

bool ActionMaker::isEnoughResToAnyBuilding() const {
	return std::ranges::any_of(nation->buildings, [this](const db_building* thing) {
		return enoughResources(thing, player);
	});
}

bool ActionMaker::isEnoughResToTypeBuilding(ParentBuildingType type) const {
	char idx = (char)type;
	return std::ranges::any_of(nation->buildings, [this, idx](const db_building* thing) {
		return thing->parentType[idx] && enoughResources(thing, player);
	});
}

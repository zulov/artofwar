#include "ActionMaker.h"

#include <numeric>
#include <unordered_set>
#include <valarray>

#include "AiUtils.h"
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "commands/action/GeneralActionCommand.h"
#include "commands/action/GeneralActionType.h"
#include "database/DatabaseCache.h"
#include "math/RandGen.h"
#include "math/VectorUtils.h"
#include "player/Player.h"
#include "player/ai/ActionCenter.h"
#include "simulation/env/Environment.h"
#include "stats/Stats.h"
#include "stats/StatsEnums.h"

using namespace magic_enum::ostream_operators;

ActionMaker::ActionMaker(Player* player): player(player),
                                          mainBrain("Data/ai/main_w.csv"),
                                          buildingBrainId("Data/ai/buildId_w.csv"),
                                          buildingBrainPos("Data/ai/buildPos_w.csv"),
                                          unitBrainId("Data/ai/unitId_w.csv"),
                                          unitBrainPos("Data/ai/unitPos_w.csv"),
                                          unitOrderId("Data/ai/unitOrderId_w.csv"),

                                          buildingLevelUpId("Data/ai/buildId_w.csv"),
                                          unitLevelUpId("Data/ai/buildId_w.csv"),
                                          unitLevelUpPos("Data/ai/buildId_w.csv") {
}

const std::vector<float>& ActionMaker::decide(Brain& brain) const {
	const auto data = Game::getStats()->getInputFor(player->getId());

	return brain.decide(data);
}

void ActionMaker::action() {
	int ids[3];
	float vals[3];
	float max = getBestThree(ids, vals, decide(mainBrain));

	if (max <= 0) {
		return;
	}
	for (int i = 0; i < 3; ++i) {
		std::cout << static_cast<StatsOutputType>(ids[i]) << "-" << (vals[i] / max * 100) << "%, ";
	}

	float val = RandGen::nextRand(RandFloatType::AI, max);
	float sum = 0;

	for (int i = 0; i < 3; ++i) {
		sum += vals[i];
		if (val <= sum) {
			std::cout << " -> " << static_cast<StatsOutputType>(ids[i]);
			auto result = createOrder(static_cast<StatsOutputType>(ids[i]));
			if (result) {
				std::cout << "(DONE)" << std::endl;
			} else {
				std::cout << std::endl;
			}
			return;
		}
	}

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
	if (enoughResources(unit)) {
		Building* building = getBuildingToDeploy(unit);
		if (building) {
			Game::getActionCenter()->add(
				new BuildingActionCommand(building, BuildingActionType::UNIT_CREATE, unit->id, player->getId()));
			return true;
		}
	}
	return false;
}

db_building* ActionMaker::chooseBuilding() {
	auto& buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
	auto& result = decide(buildingBrainId);

	std::valarray<float> center(result.data(), result.size()); //TODO perf valarraay test
	std::vector<float> diffs;
	diffs.reserve(buildings.size());
	for (auto building : buildings) {
		diffs.push_back(dist(center, player->getLevelForBuilding(building->id)->aiProps));
	}

	auto inx = randFromThree(diffs);
	if (inx < 0) {
		return nullptr;
	}
	std::cout << buildings[inx]->name.CString();
	return buildings[inx];
}

db_building_level* ActionMaker::chooseBuildingLevelUp() {
	auto& buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
	auto& result = decide(buildingLevelUpId);

	std::valarray<float> center(result.data(), result.size()); //TODO perf valarraay test
	std::vector<float> diffs;
	diffs.reserve(buildings.size());
	for (auto building : buildings) {
		auto opt = player->getNextLevelForBuilding(building->id);
		if (opt.has_value()) {
			diffs.push_back(dist(center, opt.value()->aiPropsLevelUp));
		} else {
			diffs.push_back(1000);
		}
	}

	auto inx = randFromThree(diffs);
	if (inx < 0) {
		return nullptr;
	}
	auto opt = player->getNextLevelForBuilding(buildings[inx]->id);
	if (opt.has_value()) {
		std::cout << buildings[inx]->name.CString() << " " << opt.value()->name.CString();
		return opt.value();
	}
	return nullptr;
}

db_unit* ActionMaker::chooseUnit() {
	auto& units = Game::getDatabase()->getNation(player->getNation())->units;
	auto& result = decide(unitBrainId);

	std::valarray<float> center(result.data(), result.size()); //TODO perf valarraay test
	std::vector<float> diffs;
	diffs.reserve(units.size());
	for (auto unit : units) {
		diffs.push_back(dist(center, player->getLevelForUnit(unit->id)->aiProps));
	}

	auto inx = randFromThree(diffs);
	if (inx < 0) {
		return nullptr;
	}
	std::cout << units[inx]->name.CString();
	return units[inx];
}

db_unit_level* ActionMaker::chooseUnitLevelUp() {
	auto& units = Game::getDatabase()->getNation(player->getNation())->units;
	auto& result = decide(unitLevelUpId);

	std::valarray<float> center(result.data(), result.size()); //TODO perf valarraay test
	std::vector<float> diffs;
	diffs.reserve(units.size());
	for (auto unit : units) {
		auto opt = player->getNextLevelForUnit(unit->id);
		if (opt.has_value()) {
			diffs.push_back(dist(center, opt.value()->aiPropsLevelUp));
		} else {
			diffs.push_back(1000);
		}
	}

	auto inx = randFromThree(diffs);
	if (inx < 0) {
		return nullptr;
	}
	auto opt = player->getNextLevelForUnit(units[inx]->id);
	if (opt.has_value()) {
		std::cout << units[inx]->name.CString() << " " << opt.value()->name.CString();
		return opt.value();
	}
	return nullptr;
}

float ActionMaker::dist(std::valarray<float>& center, db_ai_property* props) {
	std::valarray<float> aiAsArray(props->paramsNorm,AI_PROPS_SIZE); //TODO get as val array odrazu
	auto diff = aiAsArray - center;
	auto sq = diff * diff;
	return sq.sum();
}

const std::vector<float>& ActionMaker::inputWithParamsDecide(Brain& brain, const db_ai_property* ai_property) const {
	float input[magic_enum::enum_count<StatsInputType>() * 2 + AI_PROPS_SIZE];
	std::fill_n(input, magic_enum::enum_count<StatsInputType>() * 2 + AI_PROPS_SIZE, 0.f);

	const auto basicInput = Game::getStats()->getInputFor(player->getId());
	std::copy(basicInput, basicInput + magic_enum::enum_count<StatsInputType>() * 2, input);

	const auto aiInput = ai_property->paramsNorm;
	std::copy(aiInput, aiInput + AI_PROPS_SIZE, input + magic_enum::enum_count<StatsInputType>() * 2);

	return brain.decide(input);
}

std::optional<Urho3D::Vector2> ActionMaker::posToBuild(db_building* building) {
	auto& result = inputWithParamsDecide(buildingBrainPos, player->getLevelForBuilding(building->id)->aiProps);

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

	auto& result = inputWithParamsDecide(unitBrainPos, player->getLevelForUnit(unit->id)->aiProps);
	//TODO improve last parameter ignored queue size
	auto centers = Game::getEnvironment()->getAreas(player->getId(), result, 10);
	float closestVal = 99999;
	Building* closest = allPossible[0];
	for (auto possible : allPossible) {
		//TODO performance O(^2)
		Urho3D::Vector2 pos = {possible->getPosition().x_, possible->getPosition().z_};
		for (auto& center : centers) {
			auto diff = pos - center;
			diff = diff * diff;
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
	if (allPossible.empty()) { return nullptr; }
	auto& result = inputWithParamsDecide(unitLevelUpPos, level->aiPropsLevelUp);
	float val = result[0];
	float closestVal = 99999;
	Building* closest = allPossible[0];
	for (auto possible : allPossible) {
		
	}
}

bool ActionMaker::createOrder(StatsOutputType order) {
	switch (order) {
	case StatsOutputType::IDLE: break;
	case StatsOutputType::CREATE_UNIT:
		return createUnit();
	case StatsOutputType::CREATE_BUILDING:
		return createBuilding();
	case StatsOutputType::LEVEL_UP_UNIT:
		return levelUpUnit();
	case StatsOutputType::LEVEL_UP_BUILDING:
		return levelUpBuilding();
	default: ;
	}
	return false;
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

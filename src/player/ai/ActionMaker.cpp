#include "ActionMaker.h"

#include <numeric>
#include <unordered_set>
#include <valarray>
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "commands/action/BuildingActionType.h"
#include "database/DatabaseCache.h"
#include "math/RandGen.h"
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
                                          unitBrainPos("Data/ai/unitPos_w.csv") {
}

float* ActionMaker::decide(Brain& brain) const {
	const auto data = Game::getStats()->getInputFor(player->getId());

	return brain.decide(data);
}

void ActionMaker::action() {
	const auto result = decide(mainBrain);
	std::vector<float> v;
	v.insert(v.begin(), result, result + mainBrain.getOutputSize());
	int ids[3];
	float vals[3];


	for (int i = 0; i < 3; ++i) {
		const auto max = std::max_element(v.begin(), v.end());
		ids[i] = max - v.begin();
		vals[i] = *max;
		*max = -100;
	}
	for (auto& val : vals) {
		if (val < 0) {
			val = 0;
		}
	}
	float max = std::accumulate(vals, vals + 3, 0.f);
	if (max <= 0) {
		return;
	}
	float val = RandGen::nextRand(RandFloatType::AI, max);
	float sum = 0;
	for (int i = 0; i < 3; ++i) {
		std::cout << static_cast<StatsOutputType>(ids[i]) << "-" << (vals[i] / max * 100) << "%, ";
	}
	for (int i = 0; i < 3; ++i) {
		sum += vals[i];
		if (val <= sum) {
			std::cout << " -> " << static_cast<StatsOutputType>(ids[i]) << std::endl;
			createOrder(static_cast<StatsOutputType>(ids[i]));
			return;
		}
	}

}

bool ActionMaker::enoughResources(db_with_cost* withCosts) const {
	return withCosts && player->getResources().hasEnough(withCosts->costs);
}

void ActionMaker::createBuilding() {
	auto building = chooseBuilding();
	if (enoughResources(building)) {
		auto pos = posToBuild(building);
		if (pos.has_value()) {
			Game::getActionCenter()->addBuilding(building->id, pos.value(), player->getId());
		}
	}
}

void ActionMaker::createUnit() {
	db_unit* unit = chooseUnit();
	if (enoughResources(unit)) {
		Building* building = getBuildingToDeploy(unit);
		if (building) {
			Game::getActionCenter()->add(
				new BuildingActionCommand(building, BuildingActionType::UNIT_CREATE, unit->id, player->getId()));
		}
	}
}

std::optional<short> ActionMaker::chooseUpgrade(StatsOutputType order) const {
	//TODO perf tu jakich cahce
	auto db = Game::getDatabase();
	auto nation = db->getNation(player->getNation());
	std::vector<db_unit_level*> unitsLevels;
	for (auto unit : nation->units) {
		// for (auto unitUpgrade : *db->getUnitUpgrades(unit->id)) {
		// 	unitUpgrade->
		// }

		auto level = player->getNextLevelForUnit(unit->id);
		if (level.has_value()) {
			unitsLevels.push_back(level.value());
		}

	}
	std::vector<db_building_level*> buildingLevels;

	for (auto building : nation->buildings) {
		auto level = player->getNextLevelForBuilding(building->id);
		if (level.has_value()) {
			buildingLevels.push_back(level.value());
		}
	}

	switch (order) {
	case StatsOutputType::LEVEL_UP_BUILDING:
		if (!unitsLevels.empty()) {
			return unitsLevels.at(0)->unit;
		}
		break;
	case StatsOutputType::LEVEL_UP_UNIT:
		//if (!buildingLevels.empty()) {
		break;
	}

	return {};
}

db_building* ActionMaker::chooseBuilding() {
	auto& buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
	auto result = decide(buildingBrainId);

	std::valarray<float> center(result,AI_PROPS_SIZE); //TODO perf valarraay test
	auto closestId = buildings[0]->id;
	float closestV = 9999999;
	for (auto building : buildings) {
		closest(center, closestId, closestV, player->getLevelForBuilding(building->id)->aiProps, building->id);
	}

	return buildings[closestId];
}

db_unit* ActionMaker::chooseUnit() {
	auto& units = Game::getDatabase()->getNation(player->getNation())->units;
	auto result = decide(unitBrainId);

	std::valarray<float> center(result,AI_PROPS_SIZE); //TODO perf valarraay test
	auto closestId = units[0]->id;
	float closestV = 9999999;
	for (auto unit : units) {
		closest(center, closestId, closestV, player->getLevelForUnit(unit->id)->aiProps, unit->id);
	}

	return units[closestId];
}

void ActionMaker::closest(std::valarray<float>& center, short& closestId, float& closest, db_ai_property* props,
                          short id) {
	std::valarray<float> aiAsArray(props->paramsNorm,AI_PROPS_SIZE); //TODO get as val array odrazu
	auto diff = aiAsArray - center;
	auto sq = diff * diff;
	auto dist = sq.sum();
	if (dist < closest) {
		closest = dist;
		closestId = id;
	}
}

std::optional<Urho3D::Vector2> ActionMaker::posToBuild(db_building* building) {
	const auto level = player->getLevelForBuilding(building->id);

	float input[magic_enum::enum_count<StatsInputType>() * 2 + AI_PROPS_SIZE];
	std::fill_n(input, magic_enum::enum_count<StatsInputType>() * 2 + AI_PROPS_SIZE, 0.f);

	const auto basicInput = Game::getStats()->getInputFor(player->getId());
	std::copy(basicInput, basicInput + magic_enum::enum_count<StatsInputType>() * 2, input);

	const auto aiInput = level->aiProps->paramsNorm;
	std::copy(aiInput, aiInput + AI_PROPS_SIZE, input + magic_enum::enum_count<StatsInputType>() * 2);

	auto result = buildingBrainPos.decide(basicInput);
	return Game::getEnvironment()->getPosToCreate(building, player->getId(), result);
}

Building* ActionMaker::getBuildingToDeploy(db_unit* unit) const {
	auto& buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
	std::vector<short> buildingIdsThatCanDeploy;
	for (auto building : buildings) {
		auto unitIds = player->getLevelForBuilding(building->id)->unitsPerNationIds[player->getNation()];

		if (std::find(unitIds->begin(), unitIds->end(), unit->id) != unitIds->end()) {
			buildingIdsThatCanDeploy.push_back(building->id);
		}
	}
	std::vector<Building*> allPossible;
	for (auto thatCanDeploy : buildingIdsThatCanDeploy) {
		auto buildingEnts = player->getPossession().getBuildings(thatCanDeploy);
		allPossible.insert(allPossible.end(), buildingEnts->begin(), buildingEnts->end());
	}
	return allPossible[0];
}


void ActionMaker::createOrder(StatsOutputType order) {
	switch (order) {
	case StatsOutputType::IDLE: break;
	case StatsOutputType::CREATE_UNIT:
		createUnit();
		break;
	case StatsOutputType::CREATE_BUILDING:
		createBuilding();
		break;
	case StatsOutputType::LEVEL_UP_UNIT:
		levelUpUnit();
		break;
	case StatsOutputType::LEVEL_UP_BUILDING:
		levelUpBuilding();
		break;
	case StatsOutputType::ORDER_GO: break;
	case StatsOutputType::ORDER_STOP: break;
	case StatsOutputType::ORDER_CHARGE: break;
	case StatsOutputType::ORDER_ATTACK: break;
	case StatsOutputType::ORDER_DEAD: break;
	case StatsOutputType::ORDER_DEFEND: break;
	case StatsOutputType::ORDER_FOLLOW: break;
	case StatsOutputType::ORDER_COLLECT: break;
	default: ;
	}
}

void ActionMaker::levelUpBuilding() {
}

void ActionMaker::levelUpUnit() {
	// auto opt = chooseUnitUpgrade(order);
	// if (opt.has_value()) {
	// 	short unitId = opt.value(); //TODO lub buildingID? rodzieliæ to
	// 	Building* building = bestBuildingToUpgrade(player->getId(), unitId);
	//
	// 	Game::getActionCenter()->add(
	// 		new BuildingActionCommand(building, BuildingActionType::UNIT_LEVEL, unitId, player->getId()));
	// }	
}

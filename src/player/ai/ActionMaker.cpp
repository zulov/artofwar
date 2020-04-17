#include "ActionMaker.h"
#include "ActionCenter.h"
#include "commands/action/BuildingActionCommand.h"
#include "player/Player.h"
#include "database/DatabaseCache.h"
#include "Game.h"
#include "simulation/env/Environment.h"
#include "stats/Stats.h"
#include "stats/StatsEnums.h"
#include <valarray>


#include "AiInfluenceType.h"
#include "database/db_gets.h"


ActionMaker::ActionMaker(Player* player): player(player),
                                          mainBrain("Data/ai/w.csv"),
                                          buildingBrainId("Data/ai/w.csv"),
                                          buildingBrainPos("Data/ai/w.csv") {
}

float* ActionMaker::decide(Brain& brain) const {
	const auto data = Game::getStats()->getInputFor(player->getId());

	return brain.decide(data);
}

void ActionMaker::action() {
	const auto result = decide(mainBrain);

	const auto max = std::max_element(result, result + mainBrain.getOutputSize());

	auto index = max - result;

	createOrder(static_cast<StatsOutputType>(index));
}

void ActionMaker::createBuilding() {
	auto idToCreate = chooseBuilding();
	Urho3D::Vector2 pos = posToBuild(idToCreate);
	Game::getActionCenter()->addBuilding(idToCreate, pos, player->getId(),
	                                     player->getLevelForBuilding(idToCreate));
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

		auto level = unit->getLevel(player->getLevelForUnit(unit->id) + 1);
		if (level.has_value()) {
			unitsLevels.push_back(level.value());
		}

	}
	std::vector<db_building_level*> buildingLevels;

	for (auto building : nation->buildings) {
		auto level = building->getLevel(player->getLevelForBuilding(building->id) + 1);
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

void ActionMaker::getValues(float* values, const std::function<float(db_ai_prop_level*)>& func) const {
	int i = 0;
	auto buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
	for (auto building : buildings) {
		auto optLevel = building->getLevel(player->buildingLevels[building->id]);
		if (optLevel.has_value()) {
			values[i++] = func(optLevel.value());
		} else {
			values[i++] = -1;
		}
	}
}

short ActionMaker::chooseBuilding() {
	auto buildings = Game::getDatabase()->getNation(player->getNation())->buildings;

	auto result = decide(buildingBrainId);

	std::valarray<float> center = {result[0], result[1], result[2]}; //TODO perf valarraay test
	for (auto building : buildings) {
		//TODO bug value
		auto props = building->getLevel(player->getLevelForBuilding(building->id)).value()->aiProps;
		std::valarray<float> aiAsArray = {props->econ, props->attack, props->defence}; //TODO get as val array odrazu
		auto diff = aiAsArray - center;
		auto sq = diff * diff;
		auto dist = sq.sum();
		std::cout << dist;
	}
	std::cout << std::endl;

	return buildings.at(0)->id; //TODO bug wybrac 
}

Urho3D::Vector2 ActionMaker::posToBuild(short idToCreate) {
	auto level = getBuildingLevel(player->getId(), idToCreate);
	float input[BASIC_INPUT_SIZE + AI_PROPS_SIZE];
	std::fill_n(input,BASIC_INPUT_SIZE + AI_PROPS_SIZE, 0.f);

	const auto basicInput = Game::getStats()->getInputFor(player->getId());
	std::copy(basicInput, basicInput + BASIC_INPUT_SIZE, input);

	const auto aiInput = level->aiProps->paramsNorm;
	std::copy(aiInput, aiInput + AI_PROPS_SIZE, input + BASIC_INPUT_SIZE);

	auto result = buildingBrainPos.decide(basicInput);
	return Game::getEnvironment()->getPosToCreate(idToCreate, player->getId(), result);
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


void ActionMaker::createUnit() {
	//	short id = chooseUnit(order);
	//	auto pos = bestBuildingToDeployUnit(order, id);
}

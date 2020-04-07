#include "ActionMaker.h"
#include "ActionCenter.h"
#include "AiUtils.h"
#include "commands/action/BuildingActionCommand.h"
#include "player/Player.h"
#include "database/DatabaseCache.h"
#include "Game.h"
#include "simulation/env/Environment.h"
#include "stats/StatsEnums.h"


ActionMaker::ActionMaker(Player* player): player(player) {
}

void ActionMaker::createBuilding(StatsOutputType order) const {
	auto idToCreate = chooseBuilding(order);
	Game::getActionCenter()->addBuilding(idToCreate, bestPosToBuild(order, player->getId()), player->getId(),
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

short ActionMaker::chooseBuilding(StatsOutputType order) const {
	auto buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
	float* values = new float[buildings.size()];

	if (order == StatsOutputType::CREATE_BUILDING) {
		getValues(values, attackLevelValue);
	}
	return buildings.at(0)->id;
}

void ActionMaker::createOrder(StatsOutputType order) {
	switch (order) {
	case StatsOutputType::IDLE: break;
	case StatsOutputType::CREATE_UNIT:
		createUnit(order);
		break;
	case StatsOutputType::CREATE_BUILDING:
		createBuilding(order);
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


Urho3D::Vector2 ActionMaker::bestPosToBuild(StatsOutputType order, const short id) const {
	return Game::getEnvironment()->bestPosToBuild(player->getId(), id);
}


void ActionMaker::createUnit(StatsOutputType order) {
	//	short id = chooseUnit(order);
	//	auto pos = bestBuildingToDeployUnit(order, id);
}

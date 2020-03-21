#include "ActionMaker.h"
#include "ActionCenter.h"
#include "player/Player.h"
#include "database/DatabaseCache.h"
#include "Game.h"
#include "simulation/env/Environment.h"
ActionMaker::ActionMaker(Player* player): player(player) {}

void ActionMaker::createBuilding(StatsOutputType order) {
	auto idToCreate = chooseBuilding(order);
	Game::getActionCenter()->addBuilding(idToCreate, bestPosToBuild(order, player->getId()), player->getId(),
	                                     player->getLevelForBuilding(idToCreate));
}


std::optional<short> ActionMaker::chooseUpgrade(StatsOutputType order) {
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

	case StatsOutputType::UPGRADE_ATTACK:
		if (!unitsLevels.empty()) {
			return unitsLevels.at(0)->unit;
		}
		break;
	case StatsOutputType::UPGRADE_DEFENCE:
		//if (!buildingLevels.empty()) {
		break;
	case StatsOutputType::UPGRADE_ECON:
		//if (!buildingLevels.empty()) {
		break;
	default: ;
	}

	return {};
}

short ActionMaker::chooseBuilding(StatsOutputType order) const {
	auto buildings = Game::getDatabase()->getNation(player->getNation())->buildings;
	
	if (order == StatsOutputType::CREATE_BUILDING_ATTACK) {
		for (auto building : buildings) {
		//	float attack = Game::getDatabase()->getBuildingLevel(building->id,buildingLevels[building->id]).value().attack();
		}
	} else if (order == StatsOutputType::CREATE_BUILDING_DEFENCE) {
		for (auto building : buildings) {
			//float attack = Game::getDatabase()->getBuildingLevel(building->id,buildingLevels[building->id]).value().deffence();
		}
	} else if (order == StatsOutputType::CREATE_BUILDING_ECON) {
		for (auto building : buildings) {
		//	float attack = Game::getDatabase()->getBuildingLevel(building->id,buildingLevels[building->id]).value().econ();
			// attack = Game::getDatabase()->getBuildingLevel(building->id,buildingLevels[building->id]).value().econ();
		}
	}
	return buildings.at(0)->id;
}

void ActionMaker::createOrder(StatsOutputType order) {
	switch (order) {
	case StatsOutputType::IDLE: break;

	case StatsOutputType::CREATE_UNIT_ATTACK:
	case StatsOutputType::CREATE_UNIT_DEFENCE:
	case StatsOutputType::CREATE_UNIT_ECON:
		//	short id = chooseUnit(order);
		//	auto pos = bestBuildingToDeployUnit(order, id);
		break;
	case StatsOutputType::CREATE_BUILDING_ATTACK:
	case StatsOutputType::CREATE_BUILDING_DEFENCE:
	case StatsOutputType::CREATE_BUILDING_ECON:
		createBuilding(order);
		break;
	case StatsOutputType::UPGRADE_ATTACK:
	case StatsOutputType::UPGRADE_DEFENCE:
	case StatsOutputType::UPGRADE_ECON:
	{
		// auto opt = chooseUpgrade(order);
		// if (opt.has_value()) {
		// 	short unitId = opt.value(); //TODO lub buildingID? rodzieliæ to
		// 	Building* building = bestBuildingToUpgrade(id, unitId);
		//
		// 	Game::getActionCenter()->add(
		// 		new BuildingActionCommand(building, BuildingActionType::UNIT_LEVEL, unitId, id));
		// }
	}
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


Urho3D::Vector2 ActionMaker::bestPosToBuild(StatsOutputType order, const short id) const {
	return Game::getEnvironment()->bestPosToBuild(player->getId(), id);
}

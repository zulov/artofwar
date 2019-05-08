#include "Building.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "objects/ActionType.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include <string>
#include "consts.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include "objects/NodeUtils.h"


Building::Building(Urho3D::Vector3* _position, int id, int player, int level, int mainCell):
	Static(_position, ObjectType::BUILDING, mainCell) {
	dbBuilding = Game::getDatabaseCache()->getBuilding(id);
	upgrade(level);

	units = Game::getDatabaseCache()->getUnitsForBuilding(id);

	setPlayerAndTeam(player);
}


Building::~Building() {
	node->RemoveAllChildren();
	delete queue;
}

float Building::getMaxHpBarSize() {
	return 2.0f;
}

int Building::getDbID() {
	return dbBuilding->id;
}

void Building::populate() {
	Static::populate(dbBuilding->size);
	queue = new QueueManager(dbLevel->queueMaxCapacity);
}

void Building::absorbAttack(float attackCoef) {
	hpCoef -= attackCoef * (1 - defenseCoef);

	updateHealthBar();

	if (hpCoef < 0) {
		StateManager::changeState(this, StaticState::DEAD);
	}
}

Urho3D::String Building::toMultiLineString() {
	return Urho3D::String(dbBuilding->name + " " + dbLevel->name)
	       .Append("\nAtak: ").Append(Urho3D::String(attackCoef))
	       .Append("\nObrona: ").Append(Urho3D::String(defenseCoef))
	       .Append("\nZdrowie: ").Append(Urho3D::String((int)hpCoef)).Append("/").Append(Urho3D::String(maxHpCoef))
	       .Append("\nU¿ytkowników: ").Append(Urho3D::String((int)closeUsers))
	       .Append("/").Append(Urho3D::String((int)maxCloseUsers))
	       .Append("\nStan: ").Append(Consts::StaticStateNames[static_cast<char>(state)]);
}

void Building::action(char id, const ActionParameter& parameter) {
	Resources& resources = Game::getPlayersMan()->getActivePlayer()->getResources();

	switch (parameter.type) {
	case ActionType::UNIT_CREATE:
	{
		auto costs = Game::getDatabaseCache()->getCostForUnit(id);
		if (resources.reduce(costs)) {
			queue->add(1, parameter.type, id, 30);
		}
	}
	break;
	case ActionType::UNIT_LEVEL:
	{
		int level = Game::getPlayersMan()->getActivePlayer()->getLevelForUnit(id) + 1;
		auto opt = Game::getDatabaseCache()->getCostForUnitLevel(id, level);
		if (opt.has_value()) {
			const auto costs = opt.value();
			if (resources.reduce(costs)) {
				queue->add(1, parameter.type, id, 1);
			}
		}
	}
	break;
	case ActionType::UNIT_UPGRADE:
	{
		int level = Game::getPlayersMan()->getActivePlayer()->getLevelForUnitUpgradePath(id) + 1;
		auto opt = Game::getDatabaseCache()->getCostForUnitUpgrade(id, level);
		if (opt.has_value()) {
			const auto costs = opt.value();
			if (resources.reduce(costs)) {
				queue->add(1, parameter.type, id, 1);
			}
		}
	}
	break;
	}
}

void Building::upgrade(char level) {
	dbLevel = Game::getDatabaseCache()->getBuildingLevel(dbBuilding->id, level).value();
	loadXml("Objects/buildings/" + dbLevel->nodeName);
}

void Building::load(dbload_building* dbloadBuilding) {
	Static::load(dbloadBuilding);
//	target = {dbloadBuilding->target_x, dbloadBuilding->target_z};
}

std::string Building::getColumns() {
	return Static::getColumns() +
		"target_x		INT     NOT NULL,"
		"target_z		INT     NOT NULL";
}

std::string Building::getValues(int precision) {
//	int target_x = target.x_ * precision;
//	int target_y = target.y_ * precision;
	return Static::getValues(precision);
	//	+ std::to_string(target_x) + ","
	//	+ std::to_string(target_y);
}

int Building::getLevel() {
	return dbLevel->level;
}

void Building::createDeploy() {
	if (!getSurroundCells().empty()) {
		deployIndex = getSurroundCells().at(0);
	}
	Game::getLog()->Write(0, "createDeploy fail");
}

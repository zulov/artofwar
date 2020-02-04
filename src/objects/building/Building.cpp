#include "Building.h"
#include "Game.h"
#include "objects/ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "objects/ActionType.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "consts.h"
#include "objects/NodeUtils.h"
#include "../ValueType.h"
#include <string>


Building::Building(Urho3D::Vector3& _position, int id, int player, int level, int mainCell):
	Static(_position, mainCell) {
	setPlayerAndTeam(player);
	dbBuilding = Game::getDatabaseCache()->getBuilding(id);
	upgrade(level);
}


Building::~Building() {
	node->RemoveAllChildren();
	delete queue;
}

float Building::getMaxHpBarSize() {
	return Urho3D::Max(gridSize.x_, gridSize.y_) * 0.5;
}

int Building::getDbID() {
	return dbBuilding->id;
}

void Building::populate() {
	Static::populate(dbBuilding->size);
	queue = new QueueManager(dbLevel->queueMaxCapacity);
}

void Building::absorbAttack(float attackCoef) {
	hp -= attackCoef * (1 - defenseCoef);

	updateHealthBar();

	if (hp < 0) {
		StateManager::changeState(this, StaticState::DEAD);
	}
}

ObjectType Building::getType() const {
	return ObjectType::BUILDING;
}

Urho3D::String Building::toMultiLineString() {
	auto l10n = Game::getLocalization();

	return Urho3D::String(dbBuilding->name + " " + dbLevel->name)
		.AppendWithFormat(l10n->Get("ml_build").CString(), attackCoef, defenseCoef, (int)hp, maxHp, closeUsers,
		                  maxCloseUsers, Consts::StaticStateNames[static_cast<char>(state)]);
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
}

std::string Building::getColumns() {
	return Static::getColumns() +
		"deploy_Idx		INT     NOT NULL";
}

std::string Building::getValues(int precision) {
	return Static::getValues(precision)
		+ std::to_string(deployIndex);
}

int Building::getLevel() {
	return dbLevel->level;
}

float Building::getValueOf(ValueType type) const {
	switch (type) {
	case ValueType::ATTACK:
		return attackCoef * (hp / maxHp);
	case ValueType::DEFENCE:
		return defenseCoef * (hp / maxHp);
	}
}

void Building::createDeploy() {
	if (!getSurroundCells().empty()) {
		deployIndex = getSurroundCells().at(0);
	}
	Game::getLog()->Write(0, "createDeploy fail");
}

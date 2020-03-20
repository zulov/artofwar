#include "Building.h"
#include "Game.h"
#include "objects/ObjectEnums.h"
#include "database/DatabaseCache.h"
#include "commands/action/BuildingActionType.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "consts.h"
#include "objects/NodeUtils.h"
#include "../ValueType.h"
#include <string>
#include "objects/queue/QueueActionType.h"


Building::Building(Urho3D::Vector3& _position, int id, int player, int level, int mainCell):
	Static(_position, mainCell) {
	setPlayerAndTeam(player);
	dbBuilding = Game::getDatabase()->getBuilding(id);
	levelUp(level);
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

void Building::action(BuildingActionType type, short id) const {
	Resources& resources = Game::getPlayersMan()->getActivePlayer()->getResources();

	switch (type) {
	case BuildingActionType::UNIT_CREATE:
	{
		auto costs = Game::getDatabase()->getUnit(id)->costs;
		if (resources.reduce(costs)) {
			queue->add(1, QueueActionType::UNIT_CREATE, id, 30);
		}
	}
	break;
	case BuildingActionType::UNIT_LEVEL:
	{
		int level = Game::getPlayersMan()->getActivePlayer()->getLevelForUnit(id) + 1;
		
		auto opt = Game::getDatabase()->getUnit(id)->getLevel(level);
		if (opt.has_value()) {
			const auto costs = opt.value()->costs;
			if (resources.reduce(costs)) {
				queue->add(1, QueueActionType::UNIT_LEVEL, id, 1);
			}
		}
	}
	break;
	case BuildingActionType::UNIT_UPGRADE:
	break;
	}
}

void Building::levelUp(char level) {
	dbLevel = dbBuilding->getLevel(level).value();//TODO BUG value()
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

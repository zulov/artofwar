#include "Building.h"
#include <magic_enum.hpp>
#include <string>
#include "Game.h"
#include "../ValueType.h"
#include "commands/action/BuildingActionType.h"
#include "database/DatabaseCache.h"
#include "objects/NodeUtils.h"
#include "objects/ObjectEnums.h"
#include "objects/queue/QueueActionType.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"


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

short Building::getId() {
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
		                  maxCloseUsers, magic_enum::enum_name(state).data());
}

void Building::action(BuildingActionType type, short id) const {
	
	Resources& resources = Game::getPlayersMan()->getPlayer(getPlayer())->getResources();

	switch (type) {
	case BuildingActionType::UNIT_CREATE:
		if (resources.reduce(Game::getDatabase()->getUnit(id)->costs)) {
			queue->add(1, QueueActionType::UNIT_CREATE, id, 30);
		}
	break;
	case BuildingActionType::UNIT_LEVEL:
	{
		auto opt = Game::getPlayersMan()->getPlayer(getPlayer())->getNextLevelForUnit(id) ;
		if (opt.has_value()) {
			if (resources.reduce(opt.value()->costs)) {
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
	return dbLevel->aiProps->getValueOf(type)* (hp / maxHp);
}

void Building::createDeploy() {
	if (!getSurroundCells().empty()) {
		deployIndex = getSurroundCells().at(0);
	}
	Game::getLog()->Write(0, "createDeploy fail");
}

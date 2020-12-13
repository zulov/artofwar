#include "Building.h"
#include <magic_enum.hpp>
#include <string>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/Localization.h>
#include "Game.h"
#include "commands/action/BuildingActionType.h"
#include "database/DatabaseCache.h"
#include "objects/NodeUtils.h"
#include "objects/queue/QueueActionType.h"
#include "objects/unit/ActionParameter.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "scene/load/dbload_container.h"
#include "utils/consts.h"


Building::Building(Urho3D::Vector3 _position, int id, int player, int level, int mainCell):
	Static(_position, mainCell) {
	setPlayerAndTeam(player);
	dbBuilding = Game::getDatabase()->getBuilding(id);
	levelUp(level);
}


Building::~Building() {
	node->RemoveAllChildren();
	if (queue != &Consts::EMPTY_QUEUE) {
		delete queue;
	}
}

float Building::getMaxHpBarSize() const {
	auto gridSize = getGridSize();
	return Urho3D::Max(gridSize.x_, gridSize.y_) * 0.5f;
}

short Building::getId() {
	return dbBuilding->id;
}

char Building::getLevelNum() {
	return dbLevel->level;
}

void Building::populate() {
	Static::populate();
	if (dbLevel->queueMaxCapacity > 0) {
		queue = new QueueManager(dbLevel->queueMaxCapacity);
	} else {
		queue = &Consts::EMPTY_QUEUE;
	}
}

float Building::absorbAttack(float attackCoef) {
	auto val = attackCoef * (1 - dbLevel->armor);
	hp -= val;

	updateHealthBar();

	if (hp < 0) {
		StateManager::changeState(this, StaticState::DEAD);
	}
	return val;
}

Urho3D::String Building::toMultiLineString() {
	auto l10n = Game::getLocalization();

	return Urho3D::String(dbBuilding->name + " " + dbLevel->name)
		.AppendWithFormat(l10n->Get("ml_build").CString(), dbLevel->rangeAttackVal, dbLevel->armor, (int)hp,
		                  dbLevel->maxHp,
		                  closeUsers,
		                  getMaxCloseUsers(), magic_enum::enum_name(state).data());
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
		auto opt = Game::getPlayersMan()->getPlayer(getPlayer())->getNextLevelForUnit(id);
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
	dbLevel = dbBuilding->getLevel(level).value(); //TODO BUG value()
	loadXml("Objects/buildings/" + dbLevel->nodeName);
}

Building* Building::load(dbload_building* dbloadBuilding) {
	Static::load(dbloadBuilding);
	return this;
}

std::optional<int> Building::getDeploy() {
	if (deployIndex > -1) {
		return deployIndex;
	}
	return {};
}

std::string Building::getValues(int precision) {
	return Static::getValues(precision)
		+ std::to_string(deployIndex);
}

void Building::fillValues(std::span<float> weights) const {
	auto nation = Game::getPlayersMan()->getPlayer(player)->getNation();

	auto data = dbLevel->dbBuildingMetricPerNation[nation]->getParamsAsSpan();

	std::copy(data.begin(), data.end(), weights.begin());
	auto percent = hp / dbLevel->maxHp;
	for (auto& weight : weights) {
		weight *= percent;
	}
}

void Building::addValues(std::span<float> vals) const {
	auto percent = hp / dbLevel->maxHp;
	auto nation = Game::getPlayersMan()->getPlayer(player)->getNation();

	auto data = dbLevel->dbBuildingMetricPerNation[nation]->getParamsAsSpan();

	assert(vals.size()==data.size()-1); //without cost
	for (int i = 0; i < vals.size(); ++i) {
		vals[i] += percent * data[i];
	}
}

const Urho3D::IntVector2 Building::getGridSize() const {
	return dbBuilding->size;
}

unsigned short Building::getMaxHp() const {
	return dbLevel->maxHp;
}

void Building::createDeploy() {
	if (!surroundCells.empty()) {
		deployIndex = surroundCells.at(0);
	} else {
		deployIndex = -1;
		
	}
}

void Building::setDeploy(int cell) {
	deployIndex = cell;
}

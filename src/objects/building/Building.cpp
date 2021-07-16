#include "Building.h"
#include "commands/action/BuildingActionType.h"
#include "database/DatabaseCache.h"
#include "Game.h"
#include "objects/NodeUtils.h"
#include "objects/queue/QueueActionType.h"
#include "objects/queue/SimpleQueueManager.h"
#include "objects/unit/state/StateManager.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "player/Resources.h"
#include "scene/load/dbload_container.h"
#include <magic_enum.hpp>
#include <string>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Graphics/Material.h>
#include "objects/queue/QueueElement.h"
#include "simulation/env/Environment.h"


Building::Building(Urho3D::Vector3 _position, int id, int player, int level, int mainCell, bool withNode):
	Static(_position, mainCell, withNode) {
	setPlayerAndTeam(player);
	dbBuilding = Game::getDatabase()->getBuilding(id);
	levelUp(level);
}


Building::~Building() {
	if (node) {
		node->RemoveAllChildren();
	}
	delete queue;
}

void Building::postCreate() {
	ready = false;
	queue->add(1, QueueActionType::BUILDING_CREATE, getId(), 1);
	changeMaterial(Game::getCache()->GetResource<Urho3D::Material>("Materials/orange_overlay.xml"), model);
}

float Building::getMaxHpBarSize() const {
	const auto gridSize = getGridSize();
	return Urho3D::Max(gridSize.x_, gridSize.y_) * 0.5f;
}

float Building::getHealthBarSize() const {
	if (ready) {
		return Physical::getHealthBarSize();
	}
	return getMaxHpBarSize() * queue->getAt(0)->getProgress();
}

char Building::getLevelNum() {
	return dbLevel->level;
}

void Building::populate() {
	Static::populate();
	hp = dbLevel->maxHp;
	id = dbBuilding->id;
	if (dbLevel->queueMaxCapacity > 0) {
		queue = new QueueManager(dbLevel->queueMaxCapacity);
	} else {
		queue = new SimpleQueueManager();
	}
}

std::pair<float, bool> Building::absorbAttack(float attackCoef) {
	if (hp <= 0) {
		return {0.f, false};
	}
	const auto val = (attackCoef + attackCoef * ready) * (1 - dbLevel->armor);
	hp -= val;

	updateHealthBar();

	if (hp <= 0) {
		StateManager::changeState(this, StaticState::DEAD);
		return {val, true};
	}
	return {val, false};
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
	if (!ready) { return; }
	Resources& resources = Game::getPlayersMan()->getPlayer(getPlayer())->getResources();

	switch (type) {
	case BuildingActionType::UNIT_CREATE:
		if (resources.reduce(Game::getDatabase()->getUnit(id)->costs)) {
			queue->add(1, QueueActionType::UNIT_CREATE, id, 30);
		}
		break;
	case BuildingActionType::UNIT_LEVEL: {
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

QueueElement* Building::updateQueue(float time) const {
	return queue->update(time);
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

	std::ranges::copy(data, weights.begin());
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

float Building::getInvMaxHp() const {
	return dbLevel->invMaxHp;
}

void Building::createDeploy() {
	deployIndex = -1;
	for (auto i : surroundCells) {
		if (Game::getEnvironment()->cellIsPassable(i)) {
			deployIndex = i;
		}
	}
}

void Building::setDeploy(int cell) {
	deployIndex = cell;
}

void Building::complete() {
	ready = true;
	loadXml("Objects/buildings/" + dbLevel->nodeName);
}

float Building::getSightRadius() const {
	return dbLevel->sightRadius;
}

short Building::getCostSum() const {
	return dbBuilding->getSumCost();
}

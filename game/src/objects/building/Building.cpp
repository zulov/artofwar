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
#include "math/MathUtils.h"
#include "objects/projectile/ProjectileManager.h"
#include "objects/queue/QueueElement.h"
#include "env/Environment.h"


Building::Building(Urho3D::Vector3 _position, db_building* db_building, char playerId, char teamId, int level, int indexInGrid, UId uId):
	Static(_position, indexInGrid, uId) {
	player = playerId;
	team = teamId;
	dbBuilding = db_building;
	levelUp(level);
}


Building::~Building() {
	if (node) {
		node->RemoveAllChildren();
	}
	delete queue;
}

void Building::postCreate() {
	queue->add(1, QueueActionType::BUILDING_CREATE, getDbId(), 1);
	setShaderParam(this, "Progress", 0.0);
}

unsigned short Building::getMaxHpBarSize() const {
	const auto gridSize = getGridSize();
	return Urho3D::Max(gridSize.x_, gridSize.y_) * 0.5f;
}

float Building::getHealthBarSize() const {
	if (isReady()) {
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
	dbId = dbBuilding->id;
	invMaxHp = dbLevel->invMaxHp;
	delete queue; //TODO optimize //TODO bug co jezeli cos jets w kolejce!!!!
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
	const auto val = (attackCoef + attackCoef * !isReady()) * (1 - dbLevel->armor);
	hp -= val;

	if (hp <= 0) {
		StateManager::changeState(this, StaticState::DEAD);
		return {val, true};
	}
	return {val, false};
}

Urho3D::String Building::getInfo() const {
	const auto l10n = Game::getLocalization();

	return Urho3D::String(dbBuilding->name + " " + dbLevel->name)
		.AppendWithFormat(l10n->Get("info_build").CString(),
		                  asStringF(dbLevel->attack, 1).c_str(),
		                  asStringF(dbLevel->armor).c_str(),
		                  (int)hp, dbLevel->maxHp,
		                  closeUsers, getMaxCloseUsers(),
		                  rangeUsers, getMaxRangeUsers(),
		                  magic_enum::enum_name(state).data());
}

unsigned char Building::getMaxCloseUsers() const {
	return dbBuilding->maxUsers;
}

const Urho3D::String& Building::getName() const {
	return dbBuilding->name;
}

float Building::getAttackVal(Physical* aim) {
	return dbLevel->attack;
}

void Building::action(BuildingActionType type, short id) const {
	if (!isReady()) { return; }
	Resources* resources = Game::getPlayersMan()->getPlayer(getPlayer())->getResources();

	switch (type) {
	case BuildingActionType::UNIT_CREATE:
		if (resources->reduce(Game::getDatabase()->getUnit(id)->costs)) {
			queue->add(1, QueueActionType::UNIT_CREATE, id, 30);
		}
		break;
	case BuildingActionType::UNIT_LEVEL: {
		auto opt = Game::getPlayersMan()->getPlayer(getPlayer())->getNextLevelForUnit(id);
		if (opt.has_value()) {
			if (resources->reduce(opt.value()->costs)) {
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
	const int hpTemp = hp;
	loadXml("Objects/buildings/" + dbLevel->nodeName);
	populate();
	if (hpTemp >= 0) {
		hp = hpTemp;
	}
}

Building* Building::load(dbload_building* dbloadBuilding) {
	Static::load(dbloadBuilding);
	return this;
}

QueueElement* Building::updateQueue() {
	if (!isReady() && !SIM_GLOBALS.HEADLESS) {
		setShaderParam(this, "Progress", queue->getAt(0)->getProgress());
	}

	return queue->update();
}

void Building::updateAi(bool ifBuildingAction) {
	if (thingToInteract &&
		(!thingToInteract->isAlive() || sqDistAs2D(thingToInteract->getPosition(), position) >
			dbLevel->sqAttackRange)) {
		thingToInteract = nullptr;
		currentFrameState = 0;
	}
	if (isReady() && dbLevel->canAttack) {
		if (thingToInteract) {
			if (currentFrameState >= dbLevel->attackReload) {
				ProjectileManager::shoot(this, thingToInteract, 7, player);
				currentFrameState = 0;
			} else {
				++currentFrameState;
			}
		} else if (ifBuildingAction) {
			const auto thingsToInteract = Game::getEnvironment()->getNeighboursFromTeamNotEq(
				this, dbLevel->attackRange);
			const auto closest = Game::getEnvironment()->closestPhysicalSimple(
				this, thingsToInteract, dbLevel->attackRange);
			thingToInteract = closest;
		}
	}

}

std::optional<int> Building::getDeploy() {
	if (deployIndex > -1) {
		return deployIndex;
	}
	return {};
}

std::string Building::getValues(int precision) {
	return Static::getValues(precision) + ","
		+ std::to_string(deployIndex);
}

const Urho3D::IntVector2 Building::getGridSize() const {
	return dbBuilding->size;
}

void Building::createDeploy() {
	deployIndex = -1;
	for (auto i : getSurroundCells()) {
		if (Game::getEnvironment()->cellIsPassable(i)) {
			deployIndex = i;
		}
	}
}

void Building::setDeploy(int cell) {
	deployIndex = cell;
}

void Building::complete() {
	StateManager::changeState(this, StaticState::ALIVE);
	setShaderParam(this, "Progress", 2.0);
	//const int hpTemp = hp;
	//loadXml("Objects/buildings/" + dbLevel->nodeName);
	//hp = hpTemp;
}

float Building::getSightRadius() const {
	return dbLevel->sightRadius * (1 - 0.7f * !isReady());
}

short Building::getCostSum() const {
	return dbBuilding->getSumCost();
}

bool Building::canUse(int index) const {
	return Game::getEnvironment()->cellIsAttackable(index);
}

db_building_level* Building::getLevel() const {
	return dbLevel;
}

float Building::getModelHeight() const {
	return dbLevel->modelHeight;
}

void Building::setModelData(float modelHeight) const {
	dbLevel->modelHeight = modelHeight;
}

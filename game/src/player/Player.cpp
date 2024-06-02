#include "player/Player.h"
#include "Game.h"
#include "Possession.h"
#include "Resources.h"
#include "database/DatabaseCache.h"
#include "objects/queue/QueueActionType.h"
#include "env/Environment.h"


Player::Player(int nationId, char team, char id, int color, Urho3D::String name, bool active):
	queue(new QueueManager(1)), dbNation(Game::getDatabase()->getNation(nationId)),
	actionMaker(this, dbNation), orderMaker(this, dbNation),
	name(std::move(name)), team(team),
	color(color), id(id), active(active), resources(new Resources()),possession(new Possession(nationId)) {

	unitLevels = new char[Game::getDatabase()->getUnits().size()];
	buildingLevels = new char[Game::getDatabase()->getBuildings().size()];

	std::fill_n(unitLevels, Game::getDatabase()->getUnits().size(), 0);
	std::fill_n(buildingLevels, Game::getDatabase()->getBuildings().size(), 0);
}

Player::~Player() {
	delete[] unitLevels;
	delete[] buildingLevels;
	delete queue;
	delete resources;
	delete possession;
}

std::string Player::getValues(int precision) const {
	return std::to_string(id) + ","
		+ std::to_string(active) + ","
		+ std::to_string(team) + ","
		+ std::to_string(dbNation->id) + ",'"
		+ name.CString() + "','"
		+ std::to_string(color) + "'";
}

void Player::setResourceAmount(int resource, float amount) const {
	resources->setValue(resource, amount);
}

void Player::setResourceAmount(float amount) const {
	resources->init(amount);
}

char Player::upgradeLevel(QueueActionType type, int id) const {
	switch (type) {
	case QueueActionType::UNIT_LEVEL:
		if (Game::getDatabase()->getUnit(id)->levels.size() - 1 > unitLevels[id]) {
			unitLevels[id]++;
			return unitLevels[id];
		}
		break;
	case QueueActionType::BUILDING_LEVEL:
		if (Game::getDatabase()->getBuilding(id)->levels.size() - 1 > buildingLevels[id]) {
			buildingLevels[id]++;
			return buildingLevels[id];
		}
		break;
	case QueueActionType::UNIT_UPGRADE:
		break;
	default: ;
	}
	return -1;
}

db_unit_level* Player::getLevelForUnit(short id) const {
	return Game::getDatabase()->getUnit(id)->getLevel(unitLevels[id]).value();
}

db_building_level* Player::getLevelForBuilding(short id) const {
	return Game::getDatabase()->getBuilding(id)->getLevel(buildingLevels[id]).value();
}

std::optional<db_unit_level*> Player::getNextLevelForUnit(short id) const {
	return Game::getDatabase()->getUnit(id)->getLevel(unitLevels[id] + 1);
}

std::optional<db_building_level*> Player::getNextLevelForBuilding(short id) const {
	return Game::getDatabase()->getBuilding(id)->getLevel(buildingLevels[id] + 1);
}

void Player::addKilled(Physical* physical) const {
	possession->addKilled(physical);
}

void Player::resetScore() {
	score = -1;
}

void Player::updateResource1s() const {
	resources->update1s(possession);
}

void Player::updateResourceMonth() const {
	resources->updateMonth();
}

void Player::updateResourceYear() const {
	resources->updateYear();
}

void Player::updatePossession() {
	possession->updateAndClean(resources);
}

void Player::add(Unit* unit) const {
	possession->add(unit);
}

void Player::add(Building* building) const {
	possession->add(building);
}

int Player::getScore() {
	if (score < 0) {
		const float visibilityPercent = Game::getEnvironment()->getVisibilityScore(id);
		score = possession->getScore() + visibilityPercent * 1000.f;
	}
	return score;
}

int Player::getWorkersNumber() const {
	return possession->getWorkersNumber();
}

QueueElement* Player::updateQueue() const {
	return queue->update();
}

QueueManager* Player::getQueue() const {
	return queue;
}

void Player::aiAction() {
	actionMaker.action();
}

void Player::aiOrder() {
	orderMaker.action();
}

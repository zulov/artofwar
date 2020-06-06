#include "player/Player.h"
#include <fstream>
#include <string>
#include <utility>
#include "Game.h"
#include "commands/action/BuildingActionCommand.h"
#include "database/DatabaseCache.h"
#include "objects/building/Building.h"
#include "objects/queue/QueueActionType.h"
#include "simulation/env/Environment.h"


Player::Player(int nationId, char team, char id, int color, Urho3D::String name, bool active):
	queue(1), actionMaker(this), orderMaker(this),
	name(std::move(name)), team(team),
	color(color), id(id), active(active), possession(nationId) {
	dbNation = Game::getDatabase()->getNation(nationId);

	unitLevels = new char[Game::getDatabase()->getUnits().size()];
	buildingLevels = new char[Game::getDatabase()->getBuildings().size()];

	std::fill_n(unitLevels, Game::getDatabase()->getUnits().size(), 0);
	std::fill_n(buildingLevels, Game::getDatabase()->getBuildings().size(), 0);
}

Player::~Player() {
	delete[] unitLevels;
	delete[] buildingLevels;
}

std::string Player::getValues(int precision) const {
	return std::to_string(id) + ","
		+ std::to_string(active) + ","
		+ std::to_string(team) + ","
		+ std::to_string(dbNation->id) + ",'"
		+ name.CString() + "','"
		+ std::to_string(color) + "'";
}

void Player::setResourceAmount(int resource, float amount) {
	resources.setValue(resource, amount);
}

void Player::setResourceAmount(float amount) {
	resources.init(amount);
}

char Player::upgradeLevel(QueueActionType type, int id) {
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

float Player::getUnitsVal(ValueType value) const {
	return possession.getUnitsVal(value);
}

float Player::getBuildingsVal(ValueType value) const {
	return possession.getBuildingsVal(value);
}

void Player::updateResource() {
	resources.resetStats();
}

void Player::updatePossession() {
	possession.updateAndClean(resources);
}

void Player::add(Unit* unit) {
	possession.add(unit);
}

void Player::add(Building* building) {
	possession.add(building);
}

int Player::getScore() const {
	return possession.getScore();
}

int Player::getAttackScore() const {
	return possession.getAttackScore();
}

int Player::getDefenceScore() const {
	return possession.getDefenceScore();
}

int Player::getWorkersNumber() const {
	return possession.getWorkersNumber();
}

void Player::deactivate() {
	resources.change();
}

void Player::activate() {
	resources.change();
}

QueueElement* Player::updateQueue(float time) {
	return queue.update(time);
}

QueueManager& Player::getQueue() {
	return queue;
}

void Player::ai() {
	actionMaker.action();
	orderMaker.action();
}

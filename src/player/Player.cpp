#include "player/Player.h"
#include <fstream>
#include <string>
#include <utility>
#include "objects/building/Building.h"
#include "commands/action/BuildingActionCommand.h"
#include "database/DatabaseCache.h"
#include "Game.h"
#include "objects/queue/QueueActionType.h"
#include "simulation/env/Environment.h"
#include "stats/Stats.h"
#include "stats/StatsEnums.h"


Player::Player(int nationId, char team, char id, int color, Urho3D::String name, bool active):
	name(std::move(name)), team(team),
	color(color), id(id),
	queue(1), active(active), actionMaker(this) {
	dbNation = Game::getDatabase()->getNation(nationId);

	std::fill_n(unitLevels, UNITS_NUMBER_DB, 0);
	std::fill_n(buildingLevels, BUILDINGS_NUMBER_DB, 0);
	initAi();
}

Player::~Player() {
	delete brain;
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

int Player::getUnitsNumber() const {
	return possession.getUnitsNumber();
}

int Player::getBuildingsNumber() const {
	return possession.getBuildingsNumber();
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
	const auto data = Game::getStats()->getInputFor(getId());

	const auto result = brain->decide(data);

	auto max = std::max_element(result, result + brain->getOutputSize());

	auto index = max - result;

	actionMaker.createOrder(static_cast<StatsOutputType>(index));
}

void Player::initAi() {
	std::ifstream infile("Data/ai/w.csv");
	std::string line;
	std::vector<std::string> lines;
	while (std::getline(infile, line)) {
		lines.push_back(line);
	}

	brain = new Brain(lines);
}

#include "player/Player.h"
#include "Game.h"
#include "ActionType.h"
#include "database/DatabaseCache.h"
#include <string>
#include <utility>
#include "simulation/env/Environment.h"
#include <fstream>
#include "stats/Stats.h"
#include "ActionCenter.h"


Player::Player(int nationId, int team, char id, int color, Urho3D::String name, bool active): name(std::move(name)),
                                                                                              team(team),
                                                                                              color(color), id(id),
                                                                                              active(active),
                                                                                              queue(1) {
	dbNation = Game::getDatabaseCache()->getNation(nationId);

	std::fill_n(unitLevels, UNITS_NUMBER_DB, 0);
	std::fill_n(buildingLevels, BUILDINGS_NUMBER_DB, 0);
	std::fill_n(unitUpgradeLevels, PATH_UPGRADES_NUMBER_DB, -1);
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

char Player::upgradeLevel(ActionType type, int id) {
	switch (type) {
	case ActionType::UNIT_LEVEL:
		if (Game::getDatabaseCache()->getUnitLevels(id)->size() - 1 > unitLevels[id]) {
			unitLevels[id]++;
			return unitLevels[id];
		}
		break;
	case ActionType::BUILDING_LEVEL:
		if (Game::getDatabaseCache()->getBuildingLevels(id)->size() - 1 > buildingLevels[id]) {
			buildingLevels[id]++;
			return buildingLevels[id];
		}
		break;
	case ActionType::UNIT_UPGRADE:
		if (Game::getDatabaseCache()->getUnitUpgrades(id)->size() - 1 > unitUpgradeLevels[id]) {
			unitUpgradeLevels[id]++;
			return unitUpgradeLevels[id];
		}
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
	auto data = Game::getStats()->getInputFor(getId());
	auto result = brain->decide(data);
	// for (int i = 0; i < brain->getOutputSize(); ++i) {
	// 	std::cout << result[i] << std::endl;
	// }
	auto max = std::max_element(result, result + brain->getOutputSize());

	auto index = max - result;
	auto value = *max;

	int a = 5;
	//auto& orderData = aiRoot->getOrder();
	//execute(orderData);
}

Urho3D::Vector2 Player::bestPosToBuild(const short id) const {
	return Game::getEnvironment()->bestPosToBuild(this->id, id);
}


void Player::initAi() {
	std::ifstream infile("Data/ai/w.csv");
	std::string line;
	std::vector<std::string> lines;
	while (std::getline(infile, line)) {
		lines.push_back(line);
	}

	//brain = new Brain(2, 5, 6, 4);
	brain = new Brain(lines);

}

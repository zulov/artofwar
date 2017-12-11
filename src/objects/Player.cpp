#include "player/Player.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include <string>


Player::Player(int nationId, int team, int _id) {
	dbNation = Game::get()->getDatabaseCache()->getNation(nationId);
	resources = new Resources(0);
	id = _id;
	this->team = team;
}

Player::~Player() {
	delete resources;
}

int Player::getNation() {
	return dbNation->id;
}

int Player::getTeam() {
	return team;
}

int Player::getId() {
	return id;
}

Resources* Player::getResources() {
	return resources;
}

std::string Player::getValues(int precision) {
	return to_string(id) + "," + to_string(0) + "," + to_string(team);
}

void Player::setResourceAmount(int resource, float amount) {
	resources->setValue(resource, amount);
}

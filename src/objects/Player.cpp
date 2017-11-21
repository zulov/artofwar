#include "player/Player.h"
#include "Game.h"
#include "database/DatabaseCache.h"


Player::Player(int nationId, int team, int _id) {
	dbNation = Game::get()->getDatabaseCache()->getNation(nationId);
	resources = new Resources(100000);
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

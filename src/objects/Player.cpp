#include "player/Player.h"
#include "Game.h"
#include "database/DatabaseCache.h"


Player::Player(int nationId, int team) {
	dbNation = Game::get()->getDatabaseCache()->getNation(nationId);
	resources = new Resources(100000);

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

Resources* Player::getResources() {
	return resources;
}

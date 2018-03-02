#include "player/Player.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include <string>


Player::Player(int nationId, int team, int _id, int _color, Urho3D::String _name) {
	dbNation = Game::get()->getDatabaseCache()->getNation(nationId);
	id = _id;
	this->team = team;
	name = _name;
	color = _color;

	fill_n(unitLevels, UNITS_NUMBER_DB, 0);
	fill_n(buildingLevels, BUILDINGS_NUMBER_DB, 0);
}

Player::~Player() {
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

int Player::getColor() {
	return color;
}

Resources& Player::getResources() {
	return resources;
}

std::string Player::getValues(int precision) {
	return to_string(id) + "," + to_string(0) + "," + to_string(team) + "," + to_string(dbNation->id) + "," + name.
		CString() + "," + to_string(color);
}

void Player::setResourceAmount(int resource, float amount) {
	resources.setValue(resource, amount);
}

void Player::setResourceAmount(float amount) {
	resources.init(amount);
}

int Player::getLevelForUnit(int id) {
	return unitLevels[id];
}

int Player::getLevelForBuilding(int id) {
	return buildingLevels[id];
}

void Player::upgradeLevel(QueueType type, int id) {
	switch (type) {
	case QueueType::UNIT_LEVEL:
		{
		int maxLevels = Game::get()->getDatabaseCache()->getUnitLevels(id)->size() - 1;
		if (maxLevels > unitLevels[id]) {
			unitLevels[id]++;
		}
		}
		break;
	case QueueType::BUILDING_LEVEL:

		break;
	default: ;
	}
}

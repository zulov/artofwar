#include "player/Player.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "MenuAction.h"
#include <string>


Player::Player(int nationId, int team, int _id, int _color, Urho3D::String _name, bool _active) {
	dbNation = Game::get()->getDatabaseCache()->getNation(nationId);
	id = _id;
	this->team = team;
	name = _name;
	color = _color;
	active = _active;

	fill_n(unitLevels, UNITS_NUMBER_DB, 0);
	fill_n(buildingLevels, BUILDINGS_NUMBER_DB, 0);
	fill_n(unitUpgradeLevels, PATH_UPGRADES_NUMBER_DB, -1);
}

Player::~Player() = default;

std::string Player::getValues(int precision) {
	return to_string(id) + "," + to_string(active) + "," + to_string(team) + "," + to_string(dbNation->id) + ",'" + name.
		CString() + "','" + to_string(color) + "'";
}

void Player::setResourceAmount(int resource, float amount) {
	resources.setValue(resource, amount);
}

void Player::setResourceAmount(float amount) {
	resources.init(amount);
}

char Player::upgradeLevel(MenuAction type, int id) {
	switch (type) {
	case MenuAction::UNIT_LEVEL:
		{
		int maxLevels = Game::get()->getDatabaseCache()->getUnitLevels(id)->size() - 1;
		if (maxLevels > unitLevels[id]) {
			unitLevels[id]++;
			return unitLevels[id];
		}
		}
		break;
	case MenuAction::BUILDING_LEVEL:
		{
		int maxLevels = Game::get()->getDatabaseCache()->getBuildingLevels(id)->size() - 1;
		if (maxLevels > buildingLevels[id]) {
			buildingLevels[id]++;
			return buildingLevels[id];
		}
		}
		break;
	case MenuAction::UNIT_UPGRADE:
		{
		int maxLevels = Game::get()->getDatabaseCache()->getUnitUpgrades(id)->size() - 1;
		if (maxLevels > unitUpgradeLevels[id]) {
			unitUpgradeLevels[id]++;
			return unitUpgradeLevels[id];
		}
		}
		break;
	default: ;
	}
	return -1;
}

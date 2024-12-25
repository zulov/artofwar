#include "UnitFactory.h"
#include "Game.h"
#include "Unit.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"
#include "player/PlayersManager.h"
#include "state/StateManager.h"


UnitFactory::UnitFactory() {
	StateManager::init();
}

UnitFactory::~UnitFactory() {
	StateManager::dispose();
}

std::vector<Unit*>& UnitFactory::create(unsigned number, int id, Urho3D::Vector2& center, int playerId, int level) {
	units.clear();
	units.reserve(number);
	int y = 0;
	const int xMax = number / sqrt(number);
	const float sideSize = xMax / 2;
	auto player = Game::getPlayersMan()->getPlayer(playerId);
	while (units.size() < number) {
		for (int x = 0; x < xMax; ++x) {
			auto position = Urho3D::Vector3(x + center.x_ - sideSize, 0, y + center.y_ - sideSize);

			position.y_ = Game::getEnvironment()->getGroundHeightAt(position.x_, position.z_);

			units.push_back(new Unit(position, id, player, level));
			if (units.size() >= number) { break; }
		}
		++y;
	}
	return units;
}

std::vector<Unit*>& UnitFactory::load(dbload_unit* unit) {
	units.clear();

	auto position = Urho3D::Vector3(unit->pos_x, Game::getEnvironment()->getGroundHeightAt(unit->pos_x, unit->pos_z), unit->pos_z);

	auto newUnit = new Unit(position, unit->id_db, unit->player, unit->level);
	newUnit->load(unit);

	units.push_back(newUnit);

	return units;
}

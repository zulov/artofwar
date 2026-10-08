#include "UnitFactory.h"
#include "Game.h"
#include "Unit.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "state/StateManager.h"


UnitFactory::UnitFactory() {
	StateManager::init();
}

UnitFactory::~UnitFactory() {
	StateManager::dispose();
}

std::vector<Unit*>& UnitFactory::create(unsigned number, unsigned short id, const Urho3D::Vector2& center, unsigned char playerId) {
	units.clear();
	units.reserve(number);
	int y = 0;
	const int xMax = number / sqrt(number);
	const float sideSize = xMax / 2;
	auto player = Game::getPlayersMan()->getPlayer(playerId);
	const auto env = Game::getEnvironment();
	while (units.size() < number) {
		for (int x = 0; x < xMax; ++x) {
			auto position = Urho3D::Vector2(x + center.x_ - sideSize, y + center.y_ - sideSize);
			const auto pos3 = env->getPosWithHeightAt(position.x_, position.y_);
			units.push_back(new Unit(pos3, id, playerId, player->getTeam(), UId(player->getNextUnitId())));
			if (units.size() >= number) { break; }
		}
		++y;
	}
	return units;
}

std::vector<Unit*>& UnitFactory::load(dbload_unit* unit) {
	units.clear();

	const auto env = Game::getEnvironment();
	auto pos3 = env->getPosWithHeightAt(unit->pos_x, unit->pos_z);
	auto teamId = Game::getPlayersMan()->getPlayer(unit->player)->getTeam();
	auto newUnit = new Unit(pos3, unit->id_db, unit->player, teamId, UId(unit->uid));
	newUnit->load(unit);

	units.push_back(newUnit);

	return units;
}

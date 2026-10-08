#include "BuildingFactory.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "objects/building/Building.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"
#include "player/Player.h"
#include "player/PlayersManager.h"

Building* BuildingFactory::create(unsigned short id, const Urho3D::UShortVector2& bucketCords, const Player* player, unsigned uid) const {
	const auto db_building = Game::getDatabase()->getBuilding(id);
	const auto env = Game::getEnvironment();

	if (const auto position = env->tryGetValidPosition(db_building->size, bucketCords, true)) {
		const auto pos3 = env->getPosWithHeightAt(position->x_, position->y_);
		return new Building(pos3, db_building, player->getId(), player->getTeam(),
		                    env->getIndex(bucketCords.x_, bucketCords.y_), UId(uid));
	}

	return nullptr;
}

Building* BuildingFactory::create(unsigned short id, const Urho3D::UShortVector2& bucketCords, unsigned char playerId) const {
	auto player = Game::getPlayersMan()->getPlayer(playerId);
	return create(id, bucketCords, player, player->getNextBuildingId());
}

Building* BuildingFactory::load(dbload_building* building) const {
	auto build = create(building->id_db, { building->buc_x, building->buc_y }, 
		Game::getPlayersMan()->getPlayer(building->player), building->uid);
	if (build) {
		return build->load(building);
	}
	return nullptr;
}

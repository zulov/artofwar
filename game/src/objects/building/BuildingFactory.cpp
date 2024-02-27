#include "BuildingFactory.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "objects/building/Building.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"

Building* BuildingFactory::create(int id, const Urho3D::IntVector2& bucketCords, int level, int player) const {
	const auto db_building = Game::getDatabase()->getBuilding(id);
	const auto env = Game::getEnvironment();
	if (env->validateStatic(db_building->size, bucketCords, true)) {
		return new Building(env->getValidPosition(db_building->size, bucketCords), db_building, player, level,
		                    env->getIndex(bucketCords.x_, bucketCords.y_), !SIM_GLOBALS.HEADLESS);
	}

	return nullptr;
}

Building* BuildingFactory::load(dbload_building* building) const {
	const Urho3D::IntVector2 bucketCords(building->buc_x, building->buc_y);

	auto build = create(building->id_db, bucketCords, building->level, building->player);
	if (build) {
		return build->load(building);
	}
	return nullptr;
}

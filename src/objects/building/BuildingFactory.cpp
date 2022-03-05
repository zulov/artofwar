#include "BuildingFactory.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "objects/building/Building.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"

Building* BuildingFactory::create(int id, const Urho3D::IntVector2& bucketCords, int level, int player) const {
	const auto db_building = Game::getDatabase()->getBuilding(id);
	if (Game::getEnvironment()->validateStatic(db_building->size, bucketCords)) {
		const auto center = Game::getEnvironment()->getValidPosition(db_building->size, bucketCords);
		return new Building(Urho3D::Vector3(center.x_,
		                                    Game::getEnvironment()->getGroundHeightAt(center.x_, center.y_),
		                                    center.y_),
		                    id, player, level, Game::getEnvironment()->getIndex(bucketCords.x_, bucketCords.y_),
		                    !SIM_GLOBALS.HEADLESS);
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

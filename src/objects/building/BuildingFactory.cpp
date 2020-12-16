#include "BuildingFactory.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "objects/building/Building.h"
#include "scene/load/dbload_container.h"
#include "simulation/env/Environment.h"

Building* BuildingFactory::create(int id, Urho3D::Vector2& center, int player, const Urho3D::IntVector2& bucketCords,
                                  int level) const {
	const auto db_building = Game::getDatabase()->getBuilding(id);
	if (Game::getEnvironment()->validateStatic(db_building->size, bucketCords)) {
		return new Building(Urho3D::Vector3(center.x_, Game::getEnvironment()->getGroundHeightAt(center.x_, center.y_), center.y_),
		                    id, player, level, Game::getEnvironment()->getIndex(bucketCords.x_, bucketCords.y_));
	}

	return nullptr;

}

Building* BuildingFactory::load(dbload_building* building) const {
	const Urho3D::IntVector2 bucketCords(building->buc_x, building->buc_y);
	const auto db_building = Game::getDatabase()->getBuilding(building->id_db);

	auto center = Game::getEnvironment()->getValidPosition(db_building->size, bucketCords);
	auto build = create(building->id_db, center, building->player, bucketCords, building->level);
	if (build) {
		return build->load(building);
	}
	return nullptr;
}

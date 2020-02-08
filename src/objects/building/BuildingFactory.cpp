#include "BuildingFactory.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "objects/building/Building.h"
#include "scene/load/dbload_container.h"
#include "simulation/env/Environment.h"


BuildingFactory::BuildingFactory() {
	buildings = new std::vector<Building*>();
}

BuildingFactory::~BuildingFactory() {
	delete buildings;
}

std::vector<Building*>* BuildingFactory::
create(int id, Urho3D::Vector2& center, int player, const Urho3D::IntVector2& _bucketCords, int level) const {
	buildings->clear();
	const auto env = Game::getEnvironment();

	buildings->push_back(new Building(Urho3D::Vector3(center.x_, env->getGroundHeightAt(center.x_, center.y_),
	                                                  center.y_),
	                                  id, player, level, env->getIndex(_bucketCords.x_, _bucketCords.y_)));

	return buildings;
}

std::vector<Building*>* BuildingFactory::load(dbload_building* building) const {
	const Urho3D::IntVector2 bucketCords(building->buc_x, building->buc_y);
	const auto db_building = Game::getDatabase()->getBuilding(building->id_db);

	auto center = Game::getEnvironment()->getValidPosition(db_building->size, bucketCords);

	const auto builds = create(building->id_db, center, building->player, bucketCords, building->level);

	for (auto build : *builds) {
		build->load(building);
	}
	return builds;
}

#include "BuildingFactory.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "simulation/env/Enviroment.h"
#include "scene/load/dbload_container.h"


BuildingFactory::BuildingFactory() {
	buildings = new std::vector<Building *>();
	buildings->reserve(DEFAULT_VECTOR_SIZE);
}

BuildingFactory::~BuildingFactory() {
	delete buildings;
}

std::vector<Building*>* BuildingFactory::
create(int id, Vector2& center, int player, const IntVector2& _bucketCords, int level) {
	buildings->clear();
	float y = Game::getEnviroment()->getGroundHeightAt(center.x_, center.y_);

	Building* building = new Building(new Vector3(center.x_, y, center.y_), id, player, level, _bucketCords);
	buildings->push_back(building);

	return buildings;
}

std::vector<Building*>* BuildingFactory::load(dbload_building* building) {
	IntVector2 bucketCords(building->buc_x, building->buc_y);
	db_building* db_building = Game::getDatabaseCache()->getBuilding(building->id_db);

	Vector2 center = Game::getEnviroment()->getValidPosition(db_building->size, bucketCords);

	return create(building->id_db, center, building->player, {1, 2}, building->level);
}

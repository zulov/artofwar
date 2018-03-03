#include "BuildingFactory.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "simulation/env/Enviroment.h"


BuildingFactory::BuildingFactory() {
	buildings = new std::vector<Building *>();
	buildings->reserve(DEFAULT_VECTOR_SIZE);
}

BuildingFactory::~BuildingFactory() {
	delete buildings;
}

std::vector<Building*>* BuildingFactory::
create(int id, Vector3* center, int player, IntVector2 _bucketCords, int level) {
	buildings->clear();

	Vector3* position = new Vector3(*center);

	Building* building = new Building(position, id, player, level);
	building->setBucketPosition(_bucketCords);
	buildings->push_back(building);

	return buildings;
}

std::vector<Building*>* BuildingFactory::load(dbload_building* building) {
	buildings->clear();
	Enviroment* env = Game::get()->getEnviroment();

	IntVector2 bucketCords(building->buc_x, building->buc_y);
	db_building* db_building = Game::get()->getDatabaseCache()->getBuilding(building->id_db);

	Vector3* position = env->getValidPosition(db_building->size, bucketCords);

	Building* newBuilding = new Building(position, building->id_db, building->player, building->level);
	newBuilding->setBucketPosition(bucketCords);
	buildings->push_back(newBuilding);

	return buildings;
}

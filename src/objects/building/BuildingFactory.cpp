#include "BuildingFactory.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include "objects/building/Building.h"
#include "scene/load/dbload_container.h"
#include "simulation/env/Enviroment.h"


BuildingFactory::BuildingFactory() {
	buildings = new std::vector<Building *>();
}

BuildingFactory::~BuildingFactory() {
	delete buildings;
}

std::vector<Building*>* BuildingFactory::
create(int id, Urho3D::Vector2& center, int player, const Urho3D::IntVector2& _bucketCords, int level) {
	buildings->clear();
	float y = Game::getEnviroment()->getGroundHeightAt(center.x_, center.y_);
	auto mainCell = Game::getEnviroment()->getIndex(_bucketCords.x_, _bucketCords.y_);

	buildings->push_back(new Building(new Urho3D::Vector3(center.x_, y, center.y_), id, player, level, mainCell));

	return buildings;
}

std::vector<Building*>* BuildingFactory::load(dbload_building* building) {
	Urho3D::IntVector2 bucketCords(building->buc_x, building->buc_y);
	db_building* db_building = Game::getDatabaseCache()->getBuilding(building->id_db);

	auto center = Game::getEnviroment()->getValidPosition(db_building->size, bucketCords);

	return create(building->id_db, center, building->player, {1, 2}, building->level);
}

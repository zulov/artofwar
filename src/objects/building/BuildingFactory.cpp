#include "BuildingFactory.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>


BuildingFactory::BuildingFactory(): EntityFactory() {
	buildings = new std::vector<Building *>();
	buildings->reserve(DEFAULT_VECTOR_SIZE);
}

BuildingFactory::~BuildingFactory() {
	buildings->clear();
	delete buildings;
}

std::vector<Building*>* BuildingFactory::create(int id, Vector3* center, int player) {
	buildings->clear();
	Game* game = Game::get();
	db_building* dbBuilding = game->getDatabaseCache()->getBuilding(id);
	std::vector<db_unit*>* dbUnits = game->getDatabaseCache()->getUnitsForBuilding(id);

	Model* model = game->getCache()->GetResource<Urho3D::Model>("Models/" + dbBuilding->model);
	Material* material = game->getCache()->GetResource<Urho3D::Material>("Materials/" + dbBuilding->texture);

	Vector3* position = new Vector3(*center);



	Building* building = new Building(position, id, player);
	buildings->push_back(building);
	building->populate(dbBuilding, dbUnits);

	return buildings;
}

#include "BuildingFactory.h"
#include <Urho3D/Graphics/StaticModel.h>


BuildingFactory::BuildingFactory(): EntityFactory() {
	buildings = new std::vector<Building *>();
	buildings->reserve(DEFAULT_VECTOR_SIZE);
}

BuildingFactory::~BuildingFactory() {
	buildings->clear();
	delete buildings;
}

std::vector<Building*>* BuildingFactory::create(unsigned int number, int id, Vector3* center, SpacingType spacingType) {
	Game* game = Game::get();
	db_building* dbBuilding = game->getDatabaseCache()->getBuilding(id);
	std::vector<db_unit*>* dbUnits = game->getDatabaseCache()->getUnitsForBuilding(id);

	int xMax = number / sqrt(number);
	double space = 1;
	String modelName = "Models/" + dbBuilding->model;
	String materialName = "Materials/" + dbBuilding->texture;
	int produced = 0;
	int y = 0;
	double sideSize = xMax * space / 2;
	Model* model = game->getCache()->GetResource<Urho3D::Model>(modelName);
	Material* material = Game::get()->getCache()->GetResource<Urho3D::Material>(materialName);
	while (produced < number) {
		for (int x = 0; x < xMax; ++x) {
			Vector3* position = new Vector3(x * space + center->x_ - sideSize, 0 + center->y_,
			                                y * space + center->z_ - sideSize);

			Node* node = game->getScene()->CreateChild();
			node->SetPosition(*position);
			node->SetScale(dbBuilding->scale);
			Urho3D::StaticModel* staticModel = node->CreateComponent<Urho3D::StaticModel>();
			staticModel->SetModel(model);
			staticModel->SetMaterial(material);

			Building* building = new Building(position, node);
			buildings->push_back(building);
			building->populate(dbBuilding, dbUnits);
			++produced;
			if (produced >= number) { break; }
		}
		++y;
	}
	return buildings;
}

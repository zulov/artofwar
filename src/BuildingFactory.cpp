#include "BuildingFactory.h"
#include <Urho3D/Graphics/StaticModel.h>


BuildingFactory::BuildingFactory(): EntityFactory() {

}

BuildingFactory::~BuildingFactory() {
}

String BuildingFactory::getModelName(BuildingType building) {
	switch (building) {
	case BARRACKS:
		return "Barracks.mdl";
	case HOUSE:
		return "House.mdl";
	case TOWER:
		return "Tower.mdl";
	case ARCHERY_RANGE:
		return "Cylinder.mdl";
	}
}

String BuildingFactory::getMaterialName(BuildingType building) {
	switch (building) {
	case BARRACKS:
		return "barracks.xml";
	case HOUSE:
		return "house.xml";
	case TOWER:
		return "tower.xml";
	case ARCHERY_RANGE:
		return "cylinder.xml";
	}
}

std::vector<Building*>* BuildingFactory::create(unsigned int number, BuildingType buildingType, Vector3* center, SpacingType spacingType) {//TODO nie typ a id konkretnej jednostki
	std::vector<Building*>* buildings = new std::vector<Building *>();
	buildings->reserve(number);

	Game* game = Game::get();
	db_building* dbBuilding = game->getDatabaseCache()->getBuilding(buildingType);
	Font* font = game->getCache()->GetResource<Font>("Fonts/"+ dbBuilding->font);

	int xMax = number / sqrt(number);
	double space = 1;
	String modelName = "Models/" + dbBuilding->model;
	String materialName = "Materials/" + dbBuilding->texture;
	int produced = 0;
	int y = 0;
	double sideSize = xMax * space / 2;
	while (produced < number) {
		for (int x = 0; x < xMax; ++x) {
			Vector3* position = new Vector3(x * space + center->x_ - sideSize, 0 + center->y_, y * space + center->z_ - sideSize);

			Node* node = game->getScene()->CreateChild("Building");
			node->SetPosition(*position);

			Urho3D::StaticModel* model = node->CreateComponent<Urho3D::StaticModel>();
			model->SetModel(game->getCache()->GetResource<Urho3D::Model>(modelName));
			model->SetMaterial(Game::get()->getCache()->GetResource<Urho3D::Material>(materialName));

			Building* building = new Building(position, node);
			buildings->push_back(building);
			building->populate(dbBuilding);
			++produced;
			if (produced >= number) { break; }
		}
		++y;
	}
	return buildings;
}

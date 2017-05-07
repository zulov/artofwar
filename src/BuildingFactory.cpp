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

std::vector<Building*>* BuildingFactory::create(unsigned int number, BuildingType unitType, Vector3* center, SpacingType spacingType) {
	std::vector<Building*>* buildings = new std::vector<Building *>();
	buildings->reserve(number);

	Game* game = Game::get();
	Font* font = game->getCache()->GetResource<Font>("Fonts/Anonymous Pro.ttf");

	int yMax = number / sqrt(number);
	double space = 1;
	String modelName = "Models/" + getModelName(unitType);
	String materialName = "Materials/" + getMaterialName(unitType);
	int produced = 0;
	int y = 0;

	while (produced < number) {
		for (int x = 0; x < yMax; ++x) {
			Vector3* position = new Vector3(x * space + center->x_, 0 + center->y_, y * space + center->z_);

			Node* node = game->getScene()->CreateChild("Building");
			node->SetPosition(*position);

			Urho3D::StaticModel* model = node->CreateComponent<Urho3D::StaticModel>();
			model->SetModel(game->getCache()->GetResource<Urho3D::Model>(modelName));
			model->SetMaterial(Game::get()->getCache()->GetResource<Urho3D::Material>(materialName));

			Building* building = new Building(new Vector3(*position), node, font);
			buildings->push_back(building);
			++produced;
			if (produced >= number) { break; }
		}
		++y;
	}
	return buildings;
}

#include "UnitFactory.h"


UnitFactory::UnitFactory(): EntityFactory() {

}

UnitFactory::~UnitFactory() {
}

double UnitFactory::getSpecSize(SpacingType spacing) {
	switch (spacing) {
	case CONSTANT:
	case RANDOM:
		return INIT_SPACE;
	}
}

std::vector<Unit*>* UnitFactory::create(unsigned number, UnitType unitType, Vector3* center, SpacingType spacingType, int player) {
	std::vector<Unit*>* units = new std::vector<Unit *>();
	units->reserve(number);
	Game* game = Game::get();
	db_unit* dbUnit = game->getDatabaseCache()->getUnit(unitType);
	Font* font = game->getCache()->GetResource<Font>("Fonts/" + dbUnit->font);

	int xMax = number / sqrt(number);

	double space = getSpecSize(spacingType);
	String modelName = "Models/" + dbUnit->model;
	int produced = 0;
	int y = 0;
	double sideSize = xMax * space;
	while (produced < number) {
		for (int x = 0; x < xMax; ++x) {
			Vector3* position = new Vector3(x * space + center->x_ - sideSize, 0 + center->y_, y * space + center->z_ - sideSize);
			Node* node = game->getScene()->CreateChild("Box");
			node->SetPosition(*position);

			StaticModel* boxObject = node->CreateComponent<StaticModel>();
			boxObject->SetModel(game->getCache()->GetResource<Model>(modelName));

			Unit* newUnit = new Unit(position, node, font);
			newUnit->populate(dbUnit);
			newUnit->setPlayer(player);
			units->push_back(newUnit);
			++produced;
			if (produced >= number) { break; }
		}
		++y;
	}
	return units;
}

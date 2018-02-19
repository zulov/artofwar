#include "UnitFactory.h"


UnitFactory::UnitFactory() {
	units = new std::vector<Unit *>();
	units->reserve(DEFAULT_VECTOR_SIZE);
	StateManager::init();
}

UnitFactory::~UnitFactory() {
	delete units;
	StateManager::dispose();
}

std::vector<Unit*>* UnitFactory::create(unsigned number, int id, Vector3* center, int player, int level) {
	units->clear();

	int y = 0;
	int xMax = number / sqrt(number);
	const float sideSize = xMax / 2;

	while (units->size() < number) {
		for (int x = 0; x < xMax; ++x) {
			Vector3* position = new Vector3(x + center->x_ - sideSize, 0 + center->y_,
			                                y + center->z_ - sideSize);
			Unit* newUnit = new Unit(position, id, player, level);

			units->push_back(newUnit);
			if (units->size() >= number) { break; }
		}
		++y;
	}
	return units;
}

std::vector<Unit*>* UnitFactory::load(dbload_unit* unit) {
	units->clear();

	Vector3* position = new Vector3(unit->pos_x, 0, unit->pos_z);
	Unit* newUnit = new Unit(position, unit->id_db, unit->player, unit->level);
	newUnit->load(unit);

	units->push_back(newUnit);

	return units;
}

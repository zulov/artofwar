#include "EnviromentStrategy.h"

EnviromentStrategy::EnviromentStrategy() {}


EnviromentStrategy::~EnviromentStrategy() {}

std::vector<Unit *> EnviromentStrategy::getNeighbours(Unit * unit, std::vector<Unit *> units) {
	std::vector<Unit*> neights;
	for (int i = 0; i < units.size(); ++i) {
		if (unit == units[i]) {continue;}
		double distance = (units[i]->getPosition() - unit->getPosition()).Length();

		if (distance<separationDistance) {
			neights.push_back(units[i]);
		}
	}
	return neights;
}

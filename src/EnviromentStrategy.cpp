#include "EnviromentStrategy.h"

EnviromentStrategy::EnviromentStrategy() {
	
}


EnviromentStrategy::~EnviromentStrategy() {}

std::vector<Unit *> EnviromentStrategy::getNeighbours(Unit * unit, std::vector<Unit *> units) {
	std::vector<Unit*> neights;
	std::vector<Unit *> arrayNeight = bucketGrid->getArrayNeight(unit);

	for (int i = 0; i < arrayNeight.size(); ++i) {
		if (unit == arrayNeight[i]) {continue;}
		double distance = (arrayNeight[i]->getPosition() - unit->getPosition()).Length();

		if (distance<separationDistance) {
			neights.push_back(arrayNeight[i]);
		}
	}
	return neights;
}

void EnviromentStrategy::prepare(std::vector<Unit *> units) {
	bucketGrid = new BucketGrid(200, 200);
	bucketGrid->writeToGrid(units);
}


void EnviromentStrategy::update(Unit * unit) {
	bucketGrid->updateGrid(unit);
}

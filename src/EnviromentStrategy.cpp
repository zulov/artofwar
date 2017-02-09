#include "EnviromentStrategy.h"

EnviromentStrategy::EnviromentStrategy() {
	
}


EnviromentStrategy::~EnviromentStrategy() {}

std::vector<Unit *> *EnviromentStrategy::getNeighbours(Unit * unit, std::vector<Unit *>* units) {
	std::vector<Unit*> *neights = new std::vector<Unit *>();
	neights->reserve(15);
	std::vector<Unit *> *arrayNeight = bucketGrid->getArrayNeight(unit);

	for (int i = 0; i < arrayNeight->size(); ++i) {
		if (unit == arrayNeight->at(i)) {continue;}
		double distance = (arrayNeight->at(i)->getPosition() - unit->getPosition()).Length();

		if (distance<separationDistance) {
			neights->push_back(arrayNeight->at(i));
		}
	}
	delete arrayNeight;
	return neights;
}

void EnviromentStrategy::prepare(std::vector<Unit *>* units) {
	bucketGrid = new BucketGrid(300, 300);
	bucketGrid->writeToGrid(units);
}


void EnviromentStrategy::update(Unit * unit) {
	bucketGrid->updateGrid(unit);
}

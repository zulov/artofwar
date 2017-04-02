#include "EnviromentStrategy.h"
#include "Main.h"

EnviromentStrategy::EnviromentStrategy() {

}


EnviromentStrategy::~EnviromentStrategy() {
}

float EnviromentStrategy::getSqDistance(Vector3* unitPosition, Vector3* otherPosition) {
	return ((*otherPosition) - (*unitPosition)).LengthSquared();
}

std::vector<Unit *>* EnviromentStrategy::getNeighbours(Unit* unit, std::vector<Unit *>* units) {
	std::vector<Unit*>* neights = new std::vector<Unit *>();
	std::vector<Unit *>* arrayNeight = bucketGrid->getArrayNeight(unit);
	//std::vector<Unit *> *arrayNeight = units;

	neights->reserve(arrayNeight->size());
	double sqSeparationDistance = unit->getMaxSeparationDistance() * unit->getMaxSeparationDistance();
	Vector3* unitPosition = unit->getPosition();
	for (int i = 0; i < arrayNeight->size(); ++i) {
		Unit * neight = (*arrayNeight)[i];
		if (unit == neight) { continue; }

		Vector3* otherPosition = neight->getPosition();
		double sqDistance = getSqDistance(unitPosition, otherPosition);

		if (sqDistance < sqSeparationDistance) {
			neights->push_back(neight);
		}
	}
	//delete arrayNeight;
	return neights;
}

void EnviromentStrategy::populate(std::vector<Unit *>* units) {
	bucketGrid = new BucketGrid(BUCKET_GRID_RESOLUTION, BUCKET_GRID_SIZE);
	bucketGrid->writeToGrid(units);
}

void EnviromentStrategy::update(Unit* unit) {
	bucketGrid->updateGrid(unit);
}

void EnviromentStrategy::clear() {
	bucketGrid->clearAfterStep();
}

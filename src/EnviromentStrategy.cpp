#include "EnviromentStrategy.h"
#include "Main.h"

EnviromentStrategy::EnviromentStrategy() {
	unitGrid = new BucketGrid(BUCKET_GRID_RESOLUTION, BUCKET_GRID_SIZE);
	obstacleGrid = new BucketGrid(BUCKET_GRID_RESOLUTION_BUILD, BUCKET_GRID_SIZE_BUILD);
}


EnviromentStrategy::~EnviromentStrategy() {
}

float EnviromentStrategy::getSqDistance(Vector3* unitPosition, Vector3* otherPosition) {
	return ((*otherPosition) - (*unitPosition)).LengthSquared();
}

std::vector<Entity*>* EnviromentStrategy::getNeighbours(Unit* unit, double radius) {
	return getNeighbours(unit, unitGrid, radius);
}

std::vector<Entity*>* EnviromentStrategy::getBuildings(Unit* unit, double radius) {
	return getNeighbours(unit, obstacleGrid, radius);
}

std::vector<Entity *>* EnviromentStrategy::getNeighbours(Unit* unit, BucketGrid* bucketGrid, double radius) {
	std::vector<Entity*>* neights = new std::vector<Entity *>();
	std::vector<Entity *>* arrayNeight = bucketGrid->getArrayNeight(unit);
	//std::vector<Unit *> *arrayNeight = entities;

	neights->reserve(arrayNeight->size());
	double sqSeparationDistance = radius * radius;
	Vector3* unitPosition = unit->getPosition();
	for (int i = 0; i < arrayNeight->size(); ++i) {
		Entity* neight = (*arrayNeight)[i];
		if (unit == neight) { continue; }

		double sqDistance = getSqDistance(unitPosition, neight->getPosition());

		if (sqDistance < sqSeparationDistance) {
			neights->push_back(neight);
		}
	}
	//delete arrayNeight;
	return neights;
}

void EnviromentStrategy::update(std::vector<Unit*>* units) {
	for (int i = 0; i < units->size(); ++i) {
		Unit* unit = (*units)[i];
		unitGrid->updateGrid(unit);
	}
}

void EnviromentStrategy::update(std::vector<Building*>* buildings) {
	for (int i = 0; i < buildings->size(); ++i) {
		Building* building = (*buildings)[i];
		obstacleGrid->updateGrid(building);
	}
}

void EnviromentStrategy::clear() {
	unitGrid->clearAfterStep();
	obstacleGrid->clearAfterStep();
}

std::vector<Entity*>* EnviromentStrategy::getNeighbours(std::pair<Entity*, Entity*>* pair) {
	return unitGrid->getArrayNeight(pair);
}

std::vector<Entity*>* EnviromentStrategy::getBuildings(std::pair<Entity*, Entity*>* pair) {
	return obstacleGrid->getArrayNeight(pair);
}

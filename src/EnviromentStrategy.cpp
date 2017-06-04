#include "EnviromentStrategy.h"
#include "Main.h"

EnviromentStrategy::EnviromentStrategy() {
	allUnitGrid = new BucketGrid(BUCKET_GRID_RESOLUTION, BUCKET_GRID_SIZE);
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		teamUnitGrid[i] = new BucketGrid(BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE_ENEMY);
	}

	obstacleGrid = new BucketGrid(BUCKET_GRID_RESOLUTION_BUILD, BUCKET_GRID_SIZE_BUILD);
}


EnviromentStrategy::~EnviromentStrategy() {
}

float EnviromentStrategy::getSqDistance(Vector3* unitPosition, Vector3* otherPosition) {
	return ((*otherPosition) - (*unitPosition)).LengthSquared();
}

std::vector<Entity*>* EnviromentStrategy::getNeighbours(Unit* unit, double radius) {
	return getNeighbours(unit, allUnitGrid, radius);
}

std::vector<Entity*>* EnviromentStrategy::getNeighboursFromTeam(Unit* unit, double radius, int team, OperatorType operatorType) {
	switch (operatorType) {
	case EQUAL:
		return getNeighbours(unit, teamUnitGrid[team], radius);
	case NOT_EQUAL: {
		std::vector<Entity*>* neight = new std::vector<Entity*>();
		for (int i = 0; i < MAX_PLAYERS; ++i) {
			if (team != i) {
				std::vector<Entity*>* neight1 = getNeighbours(unit, teamUnitGrid[i], radius);
				neight->insert(neight->end(), neight1->begin(), neight1->end());
				delete neight1;
			}
		}
		return neight;
	}
	default:
		return new std::vector<Entity*>();;
	}
}

std::vector<Entity*>* EnviromentStrategy::getBuildings(Unit* unit, double radius) {
	return getNeighbours(unit, obstacleGrid, radius);
}

std::vector<Entity *>* EnviromentStrategy::getNeighbours(Unit* unit, BucketGrid* bucketGrid, double radius) {
	std::vector<Entity*>* neights = new std::vector<Entity *>();
	BucketIterator* bucketIterator = bucketGrid->getArrayNeight(unit, radius);
	//std::vector<Unit *> *arrayNeight = entities;

	neights->reserve(30);
	double sqSeparationDistance = radius * radius;
	Vector3* unitPosition = unit->getPosition();

	while (Entity* neight = bucketIterator->next()) {
		if (unit == neight) { continue; }

		double sqDistance = getSqDistance(unitPosition, neight->getPosition());

		if (sqDistance < sqSeparationDistance) {
			neights->push_back(neight);
		}
	}
	delete bucketIterator;
	//delete arrayNeight;
	return neights;
}

void EnviromentStrategy::update(std::vector<Unit*>* units) {//TODO a jakby gridy same sie aktualizowaly a jako parametr dostawa³ tylko nowe te co dosta³y nie by³ oby potrzby przechowywania numerów?
	for (int i = 0; i < units->size(); ++i) {
		Unit* unit = (*units)[i];
		allUnitGrid->updateGrid(unit, 0);
		teamUnitGrid[unit->getTeam()]->updateGrid(unit, 1);
	}
}

void EnviromentStrategy::update(std::vector<Building*>* buildings) {
	for (int i = 0; i < buildings->size(); ++i) {
		Building* building = (*buildings)[i];
		obstacleGrid->updateGrid(building, 0);
	}
}

void EnviromentStrategy::clear() {
	allUnitGrid->clearAfterStep();
	obstacleGrid->clearAfterStep();
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		teamUnitGrid[i]->clearAfterStep();
	}
}

std::vector<Entity*>* EnviromentStrategy::getNeighbours(std::pair<Entity*, Entity*>* pair) {
	return allUnitGrid->getArrayNeight(pair);
}

std::vector<Entity*>* EnviromentStrategy::getBuildings(std::pair<Entity*, Entity*>* pair) {
	return obstacleGrid->getArrayNeight(pair);
}

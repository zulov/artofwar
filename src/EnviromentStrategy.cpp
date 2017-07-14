#include "EnviromentStrategy.h"


EnviromentStrategy::EnviromentStrategy() {
	allUnitGrid = new BucketGrid(BUCKET_GRID_RESOLUTION, BUCKET_GRID_SIZE);
	for (int i = 0; i < MAX_PLAYERS; ++i) {
		teamUnitGrid[i] = new BucketGrid(BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE_ENEMY);
	}

	obstacleGrid = new BucketGrid(BUCKET_GRID_RESOLUTION_BUILD, BUCKET_GRID_SIZE_BUILD);
	resourceGrid = new BucketGrid(BUCKET_GRID_RESOLUTION_RESOURCE, BUCKET_GRID_SIZE_RESOURCE);

	gradient = new Gradient(GRADIENT_GRID_RESOLUTION, GRADIENT_GRID_SIZE);
}


EnviromentStrategy::~EnviromentStrategy() {
}

float EnviromentStrategy::getSqDistance(Vector3* unitPosition, Vector3* otherPosition) {
	return ((*otherPosition) - (*unitPosition)).LengthSquared();
}

std::vector<Physical*>* EnviromentStrategy::getNeighbours(Unit* unit, double radius) {
	return getNeighbours(unit, allUnitGrid, radius);
}

std::vector<Physical*>* EnviromentStrategy::getNeighboursFromTeam(Unit* unit, double radius, int team, OperatorType operatorType) {
	switch (operatorType) {
	case EQUAL:
		return getNeighbours(unit, teamUnitGrid[team], radius);
	case NOT_EQUAL:
		{
		std::vector<Physical*>* neight = new std::vector<Physical*>();
		for (int i = 0; i < MAX_PLAYERS; ++i) {
			if (team != i) {
				std::vector<Physical*>* neight1 = getNeighbours(unit, teamUnitGrid[i], radius);
				neight->insert(neight->end(), neight1->begin(), neight1->end());
				delete neight1;
			}
		}
		return neight;
		}
	default:
		return new std::vector<Physical*>();;
	}
}

std::vector<Physical*>* EnviromentStrategy::getBuildings(Unit* unit, double radius) {
	return getNeighbours(unit, obstacleGrid, radius);
}

std::vector<Physical *>* EnviromentStrategy::getNeighbours(Unit* unit, BucketGrid* bucketGrid, double radius) {
	std::vector<Physical*>* neights = new std::vector<Physical *>();
	neights->reserve(10);//TODO sparametryzowac

	double sqSeparationDistance = radius * radius;
	Vector3* unitPosition = unit->getPosition();
	BucketIterator* bucketIterator = bucketGrid->getArrayNeight(unit, radius, 0);
	while (Physical* neight = bucketIterator->next()) {
		if (unit == neight) { continue; }

		double sqDistance = getSqDistance(unitPosition, neight->getPosition());

		if (sqDistance < sqSeparationDistance) {
			neights->push_back(neight);
		}
	}

	return neights;
}

std::vector<Physical*>* EnviromentStrategy::getResources(Unit* unit, double radius) {
	return getNeighbours(unit, resourceGrid, radius);
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

		if (!building->isInGrandient()) {
			gradient->add(building);
			building->setInGradinet(true);
		}
	}
}

void EnviromentStrategy::update(std::vector<ResourceEntity*>* resources) {
	for (int i = 0; i < resources->size(); ++i) {
		ResourceEntity* resource = (*resources)[i];

		if (!resource->isInGrandient()) {
			gradient->add(resource);
			resource->setInGradinet(true);
		}
	}
}

Vector3* EnviromentStrategy::getRepulsiveAt(Vector3* position) {
	return gradient->getValueAt(position->x_, position->z_);
}

std::vector<Physical*>* EnviromentStrategy::getNeighbours(std::pair<Vector3*, Vector3*>* pair) {
	return allUnitGrid->getArrayNeight(pair);
}

std::vector<Physical*>* EnviromentStrategy::getBuildings(std::pair<Vector3*, Vector3*>* pair) {
	return obstacleGrid->getArrayNeight(pair);
}

double EnviromentStrategy::getGroundHeightAt(double x, double z) {
	return 0.0;
}

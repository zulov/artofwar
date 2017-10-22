#include "Enviroment.h"
#include <iostream>

Enviroment::Enviroment() {
	mainGrid = new MainGrid(BUCKET_GRID_RESOLUTION, BUCKET_GRID_SIZE, true);

	for (int i = 0; i < MAX_PLAYERS; ++i) {
		teamUnitGrid[i] = new Grid(BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE_ENEMY);
	}

	obstacleGrid = new Grid(BUCKET_GRID_RESOLUTION_BUILD, BUCKET_GRID_SIZE_BUILD);
	resourceGrid = new Grid(BUCKET_GRID_RESOLUTION_RESOURCE, BUCKET_GRID_SIZE_RESOURCE);
}


Enviroment::~Enviroment() {
	delete obstacleGrid;
	delete resourceGrid;

	delete mainGrid;
	delete[] teamUnitGrid;
}

float Enviroment::getSqDistance(Vector3* unitPosition, Vector3* otherPosition) {
	return ((*otherPosition) - (*unitPosition)).LengthSquared();
}

std::vector<Unit*>* Enviroment::getNeighbours(Unit* unit, double radius) {
	return getNeighbours(unit, mainGrid, radius);
}

std::vector<Unit*>* Enviroment::getNeighboursFromTeam(Unit* unit, double radius, int team,
                                                      OperatorType operatorType) {
	switch (operatorType) {
	case EQUAL:
		return getNeighbours(unit, teamUnitGrid[team], radius);
	case NOT_EQUAL:
		{
		std::vector<Unit*>* neight = new std::vector<Unit*>();
		for (int i = 0; i < MAX_PLAYERS; ++i) {
			if (team != i) {
				std::vector<Unit*>* neight1 = getNeighbours(unit, teamUnitGrid[i], radius);
				neight->insert(neight->end(), neight1->begin(), neight1->end());
				delete neight1;
			}
		}
		return neight;
		}
	default:
		return new std::vector<Unit*>();
	}
}

//std::vector<Physical*>* EnviromentStrategy::getBuildings(Unit* unit, double radius) {
//	return getNeighbours(unit, obstacleGrid, radius);
//}

std::vector<Unit *>* Enviroment::getNeighbours(Unit* unit, Grid* bucketGrid, double radius) {
	std::vector<Unit*>* neights = new std::vector<Unit *>();
	neights->reserve(DEFAULT_VECTOR_SIZE);//TODO sparametryzowac

	double sqSeparationDistance = radius * radius;
	Vector3* unitPosition = unit->getPosition();
	BucketIterator* bucketIterator = bucketGrid->getArrayNeight(unit, radius, 0);
	while (Unit* neight = bucketIterator->next()) {
		if (unit == neight) { continue; }

		double sqDistance = getSqDistance(unitPosition, neight->getPosition());

		if (sqDistance < sqSeparationDistance) {
			neights->push_back(neight);
		}
	}

	return neights;
}

//std::vector<Physical*>* EnviromentStrategy::getResources(Unit* unit, double radius) {
//	return getNeighbours(unit, resourceGrid, radius);
//}

void Enviroment::update(std::vector<Unit*>* units) {
	for (auto unit : (*units)) {
		mainGrid->updateGrid(unit, 0);
		teamUnitGrid[unit->getTeam()]->updateGrid(unit, 1);
	}
}

void Enviroment::update(std::vector<Building*>* buildings) {
	for (auto building : (*buildings)) {
		mainGrid->addStatic(building);
	}
}

void Enviroment::update(std::vector<ResourceEntity*>* resources) {
	for (auto resource : (*resources)) {
		mainGrid->addStatic(resource);
	}
}

//Vector2& EnviromentStrategy::getRepulsiveAt(Vector3* position) {
//	return gradient->getValueAt(position->x_, position->z_);
//}

Vector3* Enviroment::validatePosition(Vector3* position) {
	return mainGrid->getDirectionFrom(position);
}

std::vector<Physical*>* Enviroment::getNeighbours(std::pair<Vector3*, Vector3*>* pair) {
	return mainGrid->getArrayNeight(pair);
}

std::vector<Physical*>* Enviroment::getBuildings(std::pair<Vector3*, Vector3*>* pair) {
	return obstacleGrid->getArrayNeight(pair);
}

double Enviroment::getGroundHeightAt(double x, double z) {
	return 0.0;
}

bool Enviroment::validateStatic(const IntVector2& size, Vector3* pos) {
	return mainGrid->validateAdd(size, pos);
}

Vector3* Enviroment::getValidPosition(const IntVector2& size, Vector3* pos) {
	Vector3* pos2 = mainGrid->getValidPosition(size, pos);
	pos2->y_ = getGroundHeightAt(pos2->x_, pos2->z_);
	return pos2;
}

IntVector2 Enviroment::getBucketCords(const IntVector2& size, Vector3* pos) {
	return mainGrid->getBucketCords(size, pos);
}

void Enviroment::testFind(IntVector2& startV, IntVector2& goalV) {
	unordered_map<int, int> came_from;
	std::unordered_map<int, double> cost_so_far;
	mainGrid->findPath(startV, goalV, came_from, cost_so_far);
	vector<int> path = mainGrid->reconstruct_path(startV, goalV, came_from);

	mainGrid->draw_grid(2, nullptr, &came_from, nullptr);
	std::cout << std::endl << std::endl;
	mainGrid->draw_grid(2, &cost_so_far, nullptr, nullptr);
	std::cout << std::endl << std::endl;
	mainGrid->draw_grid(2, nullptr, nullptr, &path);
}

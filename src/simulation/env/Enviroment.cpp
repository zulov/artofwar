#include "Enviroment.h"
#include <iostream>
#include <chrono>

Enviroment::Enviroment(Terrain* _terrian) {
	mainGrid = new MainGrid(BUCKET_GRID_RESOLUTION, BUCKET_GRID_SIZE, true);

	for (int i = 0; i < MAX_PLAYERS; ++i) {
		teamUnitGrid[i] = new Grid(BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE_ENEMY);
	}

	obstacleGrid = new Grid(BUCKET_GRID_RESOLUTION_BUILD, BUCKET_GRID_SIZE_BUILD);
	resourceGrid = new Grid(BUCKET_GRID_RESOLUTION_RESOURCE, BUCKET_GRID_SIZE_RESOURCE);
	terrian = _terrian;
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
		mainGrid->updateGrid(unit, -1);
		teamUnitGrid[unit->getTeam()]->updateGrid(unit, unit->getTeam());
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
	return terrian->GetHeight(Vector3(x, 0, z));
}

float Enviroment::getGroundHeightPercent(float y, float x, float div) {
	float scale = terrian->GetSpacing().y_;
	Vector3 a = Vector3(x * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2, 0,
		y * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2);
	return terrian->GetHeight(a) / scale / div;
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
	auto start = std::chrono::system_clock::now();

	mainGrid->findPath(startV, goalV);
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);
	std::cout << duration.count() << std::endl;
}

void Enviroment::prepareGridToFind() {
	mainGrid->prepareGridToFind();
}

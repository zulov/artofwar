#include "Enviroment.h"
#include <iostream>
#include <chrono>
#include <simulation/env/bucket/BucketIterator.h>

Enviroment::Enviroment(Terrain* _terrian) {
	empty = new std::vector<Unit *>();
	neights = new std::vector<Unit *>();
	neights2 = new std::vector<Unit *>();
	neights->reserve(DEFAULT_VECTOR_SIZE * 2);
	neights2->reserve(DEFAULT_VECTOR_SIZE * 2);

	mainGrid = new MainGrid(BUCKET_GRID_RESOLUTION, BUCKET_GRID_SIZE, false);

	for (int i = 0; i < MAX_PLAYERS; ++i) {
		teamUnitGrid[i] = new Grid(BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE_ENEMY);
	}

	obstacleGrid = new Grid(BUCKET_GRID_RESOLUTION_BUILD, BUCKET_GRID_SIZE_BUILD);
	resourceGrid = new Grid(BUCKET_GRID_RESOLUTION_RESOURCE, BUCKET_GRID_SIZE_RESOURCE);
	terrian = _terrian;
}


Enviroment::~Enviroment() {
//	delete obstacleGrid;
//	delete resourceGrid;
//	delete mainGrid;
//	for (auto unitGrid : teamUnitGrid) {
//		delete unitGrid;
//	}
//	delete empty;
//	delete neights;
//	delete neights2;
}

std::vector<Unit*>* Enviroment::getNeighbours(Unit* unit, const double radius) {
	return getNeighbours(unit, mainGrid, radius);
}

std::vector<Unit*>* Enviroment::getNeighboursFromTeam(Unit* unit, const double radius, const int team,
                                                      const OperatorType operatorType) {
	switch (operatorType) {
	case EQUAL:
		return getNeighbours(unit, teamUnitGrid[team], radius);
	case NOT_EQUAL:
		{
		neights2->clear();
		for (int i = 0; i < MAX_PLAYERS; ++i) {
			if (team != i) {
				std::vector<Unit*>* neightLocal = getNeighbours(unit, teamUnitGrid[i], radius);
				neights2->insert(neights2->end(), neightLocal->begin(), neightLocal->end());
			}
		}
		return neights2;
		}
	default:
		return empty;
	}
}

std::vector<Unit *>* Enviroment::getNeighbours(Unit* unit, Grid* bucketGrid, double radius) const {
	neights->clear();

	double sqSeparationDistance = radius * radius;
	Vector3* unitPosition = unit->getPosition();
	BucketIterator* bucketIterator = bucketGrid->getArrayNeight(unit, radius, 0);
	while (Unit* neight = bucketIterator->next()) {
		if (unit == neight) { continue; }

		const double sqDistance = ((*unitPosition) - (*neight->getPosition())).LengthSquared();

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

void Enviroment::update(std::vector<Building*>* buildings) const {
	for (auto building : (*buildings)) {
		mainGrid->addStatic(building);
	}
}

void Enviroment::update(std::vector<ResourceEntity*>* resources) const {
	for (auto resource : (*resources)) {
		mainGrid->addStatic(resource);
	}
}

Vector3* Enviroment::validatePosition(Vector3* position) const {
	return mainGrid->getDirectionFrom(position);
}

std::vector<Physical*>* Enviroment::getNeighbours(std::pair<Vector3*, Vector3*>* pair) const {
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

Vector3 Enviroment::getValidPosForCamera(float percentX, float percentY, const Vector3& pos, float min) {
	Vector3 a = Vector3(percentX * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2, pos.y_,
	                    percentY * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2);
	const float h = terrian->GetHeight(a);
	if (h + min > pos.y_) {
		a.y_ = h + min;
	}
	return a;
}

bool Enviroment::validateStatic(const IntVector2& size, Vector3* pos) {
	return mainGrid->validateAdd(size, pos);
}

Vector3* Enviroment::getValidPosition(const IntVector2& size, const IntVector2& bucketCords) {
	const Vector2 center = mainGrid->getCenterAt(bucketCords);
	return getValidPosition(size, new Vector3(center.x_, 0, center.y_));
}

Vector3* Enviroment::getValidPosition(const IntVector2& size, Vector3* pos) {
	Vector3* pos2 = mainGrid->getValidPosition(size, pos);
	pos2->y_ = getGroundHeightAt(pos2->x_, pos2->z_);
	return pos2;
}

IntVector2 Enviroment::getBucketCords(const IntVector2& size, Vector3* pos) const {
	return mainGrid->getBucketCords(size, pos);
}

void Enviroment::testFind(IntVector2& startV, IntVector2& goalV) {
	auto start = std::chrono::system_clock::now();

	mainGrid->findPath(startV, goalV);
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);
	std::cout << duration.count() << std::endl;
}

void Enviroment::prepareGridToFind() const {
	mainGrid->prepareGridToFind();
}

content_info* Enviroment::getContentInfo(Vector2& from, Vector2& to, bool checks[], int activePlayer) const {
	from.x_ = from.x_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2;
	from.y_ = from.y_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2;

	to.x_ = to.x_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2;
	to.y_ = to.y_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2;
	//std::cout << xFrom << "-" << xTo << "$" << yFrom << "-" << yTo << std::endl;
	return mainGrid->getContentInfo(from, to, checks, activePlayer);

}

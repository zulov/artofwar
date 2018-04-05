#include "Enviroment.h"
#include <chrono>
#include <simulation/env/bucket/BucketIterator.h>

Enviroment::Enviroment(Terrain* _terrian):
	mainGrid(BUCKET_GRID_RESOLUTION, BUCKET_GRID_SIZE, false),
	resourceGrid(BUCKET_GRID_RESOLUTION_RESOURCE, BUCKET_GRID_SIZE_RESOURCE),
	obstacleGrid(BUCKET_GRID_RESOLUTION_BUILD, BUCKET_GRID_SIZE_BUILD),
	teamUnitGrid{
		{BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE_ENEMY}, {BUCKET_GRID_RESOLUTION_ENEMY, BUCKET_GRID_SIZE_ENEMY}
	} {
	neights = new std::vector<Unit *>();
	neights2 = new std::vector<Unit *>();
	neights->reserve(DEFAULT_VECTOR_SIZE * 2);
	neights2->reserve(DEFAULT_VECTOR_SIZE * 2);

	terrian = _terrian;
}


Enviroment::~Enviroment() {
	delete neights;
	delete neights2;
}

std::vector<Unit*>* Enviroment::getNeighbours(Unit* unit, const float radius) {
	return getNeighbours(unit, mainGrid, radius);
}

std::vector<Unit*>* Enviroment::getNeighboursFromTeam(Unit* unit, const float radius, const int team,
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
	}
}

std::vector<Unit *>* Enviroment::getNeighbours(Unit* unit, Grid& bucketGrid, float radius) {
	neights->clear();

	Vector3* unitPosition = unit->getPosition();
	BucketIterator& bucketIterator = bucketGrid.getArrayNeight(unit, radius, 0);
	const float sqSepDistance = radius * radius;

	while (Unit* neight = bucketIterator.next()) {
		if (unit == neight) { continue; }
		const float xDiff = unitPosition->x_ - neight->getPosition()->x_;
		const float zDiff = unitPosition->z_ - neight->getPosition()->z_;

		const float sqDistance = xDiff * xDiff + zDiff * zDiff;

		if (sqDistance < sqSepDistance) {
			neights->push_back(neight);
		}
	}

	return neights;
}

void Enviroment::update(std::vector<Unit*>* units) {
	//TODO to mozna rodzielic na dodawanei u usywanie
	for (auto unit : *units) {
		mainGrid.updateGrid(unit, -1);
		teamUnitGrid[unit->getTeam()].updateGrid(unit, unit->getTeam());
	}
}

void Enviroment::update(std::vector<Building*>* buildings) {
	for (auto building : *buildings) {
		mainGrid.addStatic(building);
	}
}

void Enviroment::update(std::vector<ResourceEntity*>* resources) {
	for (auto resource : *resources) {
		mainGrid.addStatic(resource);
	}
}

Vector2 Enviroment::repulseObstacle(Unit* unit) {
	return mainGrid.repulseObstacle(unit);
}

Vector2* Enviroment::validatePosition(Vector3* position) {
	return mainGrid.getDirectionFrom(position);
}

std::vector<Physical*>* Enviroment::getNeighbours(std::pair<Vector3*, Vector3*>& pair) {
	return mainGrid.getArrayNeight(pair);
}

std::vector<Physical*>* Enviroment::getBuildings(std::pair<Vector3*, Vector3*>& pair) {
	return obstacleGrid.getArrayNeight(pair);
}

float Enviroment::getGroundHeightAt(float x, float z) {
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

bool Enviroment::validateStatic(const IntVector2& size, Vector2& pos) {
	return mainGrid.validateAdd(size, pos);
}

Vector2 Enviroment::getValidPosition(const IntVector2& size, const IntVector2& bucketCords) {
	const Vector2 center = mainGrid.getCenterAt(bucketCords);
	return getValidPosition(size, Vector2(center));
}

Vector2& Enviroment::getCenter(int index) {
	return mainGrid.getCenter(index);
}

void Enviroment::invalidateCache() {
	mainGrid.invalidateCache();
}

Vector2 Enviroment::getValidPosition(const IntVector2& size, Vector2& pos) {
	return mainGrid.getValidPosition(size, pos);
}

IntVector2 Enviroment::getBucketCords(const IntVector2& size, Vector2& pos) {
	return mainGrid.getBucketCords(size, pos);
}

std::vector<int>* Enviroment::findPath(int startIdx, Vector2& aim) {
	return mainGrid.findPath(startIdx, aim);
}

void Enviroment::prepareGridToFind() {
	mainGrid.prepareGridToFind();
}

content_info* Enviroment::getContentInfo(Vector2 from, Vector2 to, bool checks[], int activePlayer) {
	from.x_ = from.x_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2;
	from.y_ = from.y_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2;

	to.x_ = to.x_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2;
	to.y_ = to.y_ * BUCKET_GRID_SIZE - BUCKET_GRID_SIZE / 2;

	return mainGrid.getContentInfo(from, to, checks, activePlayer);

}

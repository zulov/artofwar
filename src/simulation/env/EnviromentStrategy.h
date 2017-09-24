#pragma once

#include <vector>
#include "simulation/env/bucket/BucketIterator.h"
#include "simulation/env/bucket/BucketGrid.h"
#include "objects/building/Building.h"
#include "defines.h"
#include "OperatorType.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "objects/Physical.h"


class EnviromentStrategy
{
public:
	EnviromentStrategy();
	~EnviromentStrategy();
	float getSqDistance(Vector3* unitPosition, Vector3* otherPosition);
	std::vector<Unit *>* getNeighbours(Unit* unit, double radius);
	std::vector<Unit *>* getNeighboursFromTeam(Unit* unit, double radius, int team, OperatorType operatorType);
//	std::vector<Physical *>* getBuildings(Unit* unit, double radius);
//
	std::vector<Unit *>* getNeighbours(Unit* unit, BucketGrid* bucketGrid, double radius);

	//std::vector<Physical*>* getResources(Unit* unit, double radius);

	void update(std::vector<Unit*>* units);
	void update(std::vector<Building*>* buildings);
	void update(std::vector<ResourceEntity*>* resources);
	void add(Entity *entity);
	Vector3* validatePosition(Vector3 * position);

	std::vector<Physical *>* getNeighbours(std::pair<Vector3*, Vector3*>* pair);
	std::vector<Physical *>* getBuildings(std::pair<Vector3*, Vector3*>* pair);
	double getGroundHeightAt(double x, double z);

private:
	BucketGrid* allUnitGrid;
	BucketGrid* teamUnitGrid[MAX_PLAYERS];
	BucketGrid* obstacleGrid;
	BucketGrid* resourceGrid;
};

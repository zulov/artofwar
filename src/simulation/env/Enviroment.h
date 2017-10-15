#pragma once

#include <vector>
#include "simulation/env/bucket/BucketIterator.h"
#include "simulation/env/bucket/Grid.h"
#include "objects/building/Building.h"
#include "defines.h"
#include "OperatorType.h"
#include "objects/resource/ResourceEntity.h"
#include "objects/unit/Unit.h"
#include "objects/Physical.h"
#include "bucket/MainGrid.h"


class Enviroment
{
public:
	Enviroment();
	~Enviroment();
	float getSqDistance(Vector3* unitPosition, Vector3* otherPosition);
	std::vector<Unit *>* getNeighbours(Unit* unit, double radius);
	std::vector<Unit *>* getNeighboursFromTeam(Unit* unit, double radius, int team, OperatorType operatorType);
//	std::vector<Physical *>* getBuildings(Unit* unit, double radius);

	std::vector<Unit *>* getNeighbours(Unit* unit, Grid* bucketGrid, double radius);

	//std::vector<Physical*>* getResources(Unit* unit, double radius);

	void update(std::vector<Unit*>* units);
	void update(std::vector<Building*>* buildings);
	void update(std::vector<ResourceEntity*>* resources);

	Vector3* validatePosition(Vector3 * position);

	std::vector<Physical *>* getNeighbours(std::pair<Vector3*, Vector3*>* pair);
	std::vector<Physical *>* getBuildings(std::pair<Vector3*, Vector3*>* pair);
	double getGroundHeightAt(double x, double z);
	bool validateStatic(db_building* dbBuilding, Vector3* pos);

private:
	MainGrid* mainGrid;
	Grid* teamUnitGrid[MAX_PLAYERS];
	Grid* obstacleGrid;
	Grid* resourceGrid;
};

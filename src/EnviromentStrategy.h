#pragma once

#include <vector>
#include "Unit.h"
#include "BucketIterator.h"
#include "BucketGrid.h"

#include "Building.h"
#include "defines.h"
#include <vector>
#include "OperatorType.h"

class EnviromentStrategy
{
public:
	EnviromentStrategy();
	~EnviromentStrategy();
	float getSqDistance(Vector3* unitPosition, Vector3* otherPosition);
	std::vector<Entity *>* getNeighbours(Unit* unit, double radius);
	std::vector<Entity *>* getNeighboursFromTeam(Unit* unit, double radius, int team, OperatorType operatorType);
	std::vector<Entity *>* getBuildings(Unit* unit, double radius);
	std::vector<Entity *>* getNeighbours(Unit* unit, BucketGrid* bucketGrid, double radius);
	void update(std::vector<Unit*>* units);
	void update(std::vector<Building*>* buildings);
	void clear();
	std::vector<Entity *>* getNeighbours(std::pair<Entity*, Entity*>* pair);
	std::vector<Entity *>* getBuildings(std::pair<Entity*, Entity*>* pair);
private:
	BucketGrid* allUnitGrid;
	BucketGrid* teamUnitGrid[MAX_PLAYERS];
	BucketGrid* obstacleGrid;
};

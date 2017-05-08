#pragma once

#include <vector>
#include "Unit.h"
#include "BucketGrid.h"
#include "Building.h"
#include <vector>


class EnviromentStrategy
{
public:
	EnviromentStrategy();
	~EnviromentStrategy();
	float getSqDistance(Vector3* unitPosition, Vector3* otherPosition);
	std::vector<Entity *>* getNeighbours(Unit* unit);
	std::vector<Entity *>* getBuildings(Unit* unit);
	std::vector<Entity *>* getNeighbours(Unit* unit, BucketGrid* bucketGrid);
	void update(std::vector<Unit*>* units);
	void update(std::vector<Building*>* buildings);
	void clear();
	std::vector<Entity *>* getNeighbours(std::pair<Entity*, Entity*>* pair);
	std::vector<Entity *>* getBuildings(std::pair<Entity*, Entity*>* pair);
private:
	BucketGrid* unitGrid;
	BucketGrid* obstacleGrid;
};

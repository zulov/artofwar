#pragma once

#include <vector>
#include "BucketIterator.h"
#include "BucketGrid.h"
#include "Building.h"
#include "defines.h"
#include "OperatorType.h"
#include "ResourceEntity.h"
#include "Gradient.h"


class EnviromentStrategy
{
public:
	EnviromentStrategy();
	~EnviromentStrategy();
	float getSqDistance(Vector3* unitPosition, Vector3* otherPosition);
	std::vector<Physical *>* getNeighbours(Unit* unit, double radius);
	std::vector<Physical *>* getNeighboursFromTeam(Unit* unit, double radius, int team, OperatorType operatorType);
	std::vector<Physical *>* getBuildings(Unit* unit, double radius);

	std::vector<Physical *>* getNeighbours(Unit* unit, BucketGrid* bucketGrid, double radius);

	std::vector<Physical*>* getResources(Unit* unit, double radius);

	void update(vector<Unit*>* units);
	void update(vector<Building*>* buildings);
	void update(vector<ResourceEntity*>* resources);
	void add(Entity *entity);
	Vector3 * getRepulsiveAt(Vector3 * position);

	std::vector<Physical *>* getNeighbours(std::pair<Vector3*, Vector3*>* pair);
	std::vector<Physical *>* getBuildings(std::pair<Vector3*, Vector3*>* pair);
	double getGroundHeightAt(double x, double z);
private:
	BucketGrid* allUnitGrid;
	BucketGrid* teamUnitGrid[MAX_PLAYERS];
	BucketGrid* obstacleGrid;
	BucketGrid* resourceGrid;
	Gradient * gradient;
};

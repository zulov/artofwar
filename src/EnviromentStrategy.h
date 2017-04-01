#pragma once

#include <vector>
#include "Unit.h"
#include "BucketGrid.h"

#define BUCKET_GRID_RESOLUTION 75
#define BUCKET_GRID_SIZE 300

class EnviromentStrategy {

public:
	EnviromentStrategy();
	~EnviromentStrategy();
	float getSqDistance(Vector3* unitPosition, Vector3* otherPosition);

	std::vector<Unit *> *getNeighbours(Unit * unit, std::vector<Unit *> *units);
	void populate(std::vector<Unit *> *units);
	void update(Unit * unit);
	void clear();
private:
	//const double separationDistance = 4;
	BucketGrid* bucketGrid;
};


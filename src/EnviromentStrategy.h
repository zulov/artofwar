#pragma once

#include <vector>
#include "Unit.h"
#include "BucketGrid.h"

#define BUCKET_GRID_RESOLUTION 150
#define BUCKET_GRID_SIZE 300

class EnviromentStrategy {

public:
	EnviromentStrategy();
	~EnviromentStrategy();
	
	std::vector<Unit *> *getNeighbours(Unit * unit, std::vector<Unit *> *units);
	void populate(std::vector<Unit *> *units);
	void update(Unit * unit);
	void clear();
private:
	const double separationDistance = 4;
	BucketGrid* bucketGrid;
};


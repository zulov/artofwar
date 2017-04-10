#pragma once

#include <vector>
#include "Unit.h"
#include "BucketGrid.h"


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
	BucketGrid* bucketGrid;
};


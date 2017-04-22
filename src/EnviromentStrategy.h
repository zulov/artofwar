#pragma once

#include <vector>
#include "Unit.h"
#include "BucketGrid.h"


class EnviromentStrategy {

public:
	EnviromentStrategy();
	~EnviromentStrategy();
	float getSqDistance(Vector3* unitPosition, Vector3* otherPosition);

	std::vector<Entity *> *getNeighbours(Unit * unit, std::vector<Unit *> *units);
	void populate(std::vector<Unit *> *units);
	void update(Unit * unit);
	void clear();
	std::vector<Entity *> * getNeighbours(std::pair<Entity*, Entity*>* pair);
private:
	BucketGrid* bucketGrid;
};


#pragma once
#include <iostream>
#include <vector>
#include "Unit.h"
#include "BucketGrid.h"

class EnviromentStrategy {

public:
	EnviromentStrategy();
	~EnviromentStrategy();
	
	std::vector<Unit *> getNeighbours(Unit * unit, std::vector<Unit *> units);
	void prepare(std::vector<Unit *> units);
	void update(Unit * unit);
private:
	const double separationDistance = 3;
	BucketGrid* bucketGrid;
};


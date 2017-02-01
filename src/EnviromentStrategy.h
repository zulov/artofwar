#pragma once
#include <iostream>
#include <vector>
#include "Unit.h"

class EnviromentStrategy {

public:
	EnviromentStrategy();
	~EnviromentStrategy();
	
	std::vector<Unit *> getNeighbours(Unit * unit, std::vector<Unit *> units);
private:
	const double separationDistance = 2;
};


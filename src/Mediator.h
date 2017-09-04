#pragma once
#include "simulation/env/EnviromentStrategy.h"

class Controls;

class Mediator
{
public:
	Mediator(EnviromentStrategy* _enviromentStrategy, Controls* _controls);
	~Mediator();
	vector<Physical *>* getEntities(std::pair<Vector3*, Vector3*>* held);
private:
	EnviromentStrategy* enviromentStrategy;
	Controls* controls;
};

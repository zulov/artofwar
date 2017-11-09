#pragma once
#include "simulation/env/Enviroment.h"

class Controls;

class Mediator
{
public:
	Mediator(Enviroment* _enviromentStrategy, Controls* _controls);
	~Mediator();
	std::vector<Physical *>* getEntities(std::pair<Vector3*, Vector3*>* held);
private:
	Enviroment* enviromentStrategy;
	Controls* controls;
};

#pragma once
#include "EnviromentStrategy.h"

class Controls;

class Mediator
{
public:
	Mediator(EnviromentStrategy* _enviromentStrategy, Controls* _controls);
	~Mediator();
	std::vector<Entity *>* getEntities(std::pair<Vector3*, Vector3*>* held);
private:
	EnviromentStrategy* enviromentStrategy;
	Controls* controls;
};

#include "Mediator.h"


Mediator::Mediator(EnviromentStrategy* _enviromentStrategy, Controls* _controls) {
	enviromentStrategy = _enviromentStrategy;
	controls = _controls;
}


Mediator::~Mediator() {
}

std::vector<Entity*> *Mediator::getEntities(std::pair<Vector3*, Vector3*>* held) {
	return enviromentStrategy->getNeighbours(held);
}

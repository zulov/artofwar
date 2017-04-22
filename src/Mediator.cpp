#include "Mediator.h"


Mediator::Mediator(EnviromentStrategy* _enviromentStrategy, Controls* _controls) {
	enviromentStrategy = _enviromentStrategy;
	controls = _controls;
}


Mediator::~Mediator() {
}

std::vector<Entity*> *Mediator::getEntities(std::pair<Entity*, Entity*>* held) {
	return enviromentStrategy->getNeighbours(held);
}

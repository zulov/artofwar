#include "Mediator.h"


Mediator::Mediator(Enviroment* _enviromentStrategy, Controls* _controls) {
	enviromentStrategy = _enviromentStrategy;
	controls = _controls;
}


Mediator::~Mediator() {
}

std::vector<Physical*> *Mediator::getEntities(std::pair<Vector3*, Vector3*>* held) {
	return enviromentStrategy->getNeighbours(held);
}

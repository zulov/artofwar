#include "AimContainer.h"
#define AIMS_CAPACITY 20

AimContainer::AimContainer() {
	aims = new std::vector<Aims*>();
	aims->reserve(AIMS_CAPACITY);
	for (int i = 0; i < AIMS_CAPACITY; ++i) {
		aims->push_back(new Aims(0));
	}
	index = 0;
}


AimContainer::~AimContainer() {
}

void AimContainer::clean() {
	for (int i = 0; i < aims->size(); ++i) {
		(*aims)[i]->clearAims();
	}
}

Aims* AimContainer::getNext() {
	for (int i = 0; i < aims->size(); ++i) {
		if ((*aims)[i]->getReferences() == 0) {
			(*aims)[i]->clearAims();//TODO moze nie potrzebne
			index = i;
			return (*aims)[index];
		}
	}
	aims->push_back(new Aims(0));
	index = aims->size() - 1;
	return (*aims)[index];
}

Aims* AimContainer::getCurrent() {
	return (*aims)[index];
}

#include "AimContainer.h"
#include "defines.h"

AimContainer::AimContainer() {
	aims.reserve(DEFAULT_VECTOR_SIZE);
	for (int i = 0; i < DEFAULT_VECTOR_SIZE; ++i) {
		aims.push_back(new Aims());
	}
	index = 0;
}


AimContainer::~AimContainer() {
	clear_vector(aims);
}

void AimContainer::clean() {
	for (auto aim : aims) {
		aim->clearAims();
	}
}

Aims* AimContainer::getNext() {
	for (int i = 0; i < aims.size(); ++i) {
		if (aims[i]->getReferences() == 0) {
			aims[i]->clearAims();//TODO moze nie potrzebne
			index = i;
			return aims[index];
		}
	}
	aims.push_back(new Aims());
	index = aims.size() - 1;
	return aims[index];
}

Aims* AimContainer::getCurrent() {
	return aims[index];
}

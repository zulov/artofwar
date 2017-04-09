#pragma once
#include <vector>

template< typename T >
void clear_vector(std::vector<T*>* container) {
	for (void* obj : *container) {
		delete obj;
	}
	container->clear();
}

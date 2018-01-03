#pragma once
#include <vector>

template <typename T>
void clear_vector(std::vector<T*>* container) {
	for (T* obj : *container) {
		if (obj) {
			delete obj;
		}
	}
	container->clear();
}

template <typename T>
void clear_vector(std::vector<T*>& container) {
	for (T* obj : container) {
		if (obj) {
			delete obj;
		}
	}
	container.clear();
}

template <typename T>
void clear_and_delete_vector(std::vector<T*>* container) {
	for (T* obj : *container) {
		if (obj) {
			delete obj;
		}
	}
	container->clear();
	delete container;
}


template <typename T>
void clear_delete_null_vector(std::vector<T*>** container) {
	for (T* obj : **container) {
		if (obj) {
			delete obj;
		}
	}
	(*container)->clear();
	delete container;
	container = nullptr;
}



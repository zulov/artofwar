#pragma once
#include <vector>

template <typename T>
void clear_vector(std::vector<T*>* container) {
	for (T* obj : *container) {
		delete obj;
	}
	container->clear();
}

template <typename T, size_t SIZE>
void clear_array(std::array<T*, SIZE>& container) {
	for (T* obj : container) {
		delete obj;
	}
}

template <typename T>
void clear_vector(std::vector<T*>& container) {
	for (T* obj : container) {
		delete obj;
	}
	container.clear();
}

template <typename T>
void clear_vector_array(std::vector<T*>& container) {
	for (T* obj : container) {
		delete[] obj;
	}
	container.clear();
}

template <typename T>
void clear_and_delete_vector(std::vector<T*>* container) {
	for (int i = container->size() - 1; i >= 0; --i) {
		delete container->at(i);
	}
	container->clear();
	delete container;
}

template <typename T>
void clear_delete_null_vector(std::vector<T*>** container) {
	for (T* obj : **container) {
		delete obj;
	}
	(*container)->clear();
	delete container;
	container = nullptr;
}

template <typename T>
void clear_array(T** tab, int size) {
	for (int i = 0; i < size; ++i) {
		delete tab[i];
	}
}

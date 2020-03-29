#pragma once
#include <vector>

template <typename T>
void clear_vector(std::vector<T*>* container) {
	for (T* obj : *container) {
		delete obj;
	}
	container->clear();
}

template <typename T>
void clear_vector(std::vector<T*>& container) {
	for (T* obj : container) {
		delete obj;
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

// template <typename T>
// void ensureSize(int size, std::vector<T*>& array) {
// 	if (array.size() <= size) {
// 		array.resize(size + 1, nullptr);
// 	}
// }

template <typename T>
static void setEntity(const int id, std::vector<T*>& array, T* entity) {
	ensureSize(id, array);

	array[id] = entity;
}

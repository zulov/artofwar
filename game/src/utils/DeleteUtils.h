#pragma once
#include <algorithm>
#include <vector>

template <typename T, typename Predicate>
void eraseAndDeleteIf(std::vector<T*>& items, Predicate&& predicate) {
	const auto newEnd = std::remove_if(items.begin(), items.end(), [&](T* item) {
		if (!predicate(item)) {
			return false;
		}
		delete item;
		return true;
	});
	items.erase(newEnd, items.end());
}

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
	for (auto it = container->rbegin(); it != container->rend(); ++it) {
		delete *it;
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

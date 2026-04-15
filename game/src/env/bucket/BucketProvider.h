#pragma once
#include <vector>

template <typename T>
class ArrayProvider {
public:
	~ArrayProvider() {
		for (auto& entry : pool) {
			delete[] entry.data;
		}
		pool.clear();
	}

	static T* get(int size) {
		for (auto it = pool.begin(); it != pool.end(); ++it) {
			if (it->size == size) {
				T* data = it->data;
				pool.erase(it);
				return data;
			}
		}
		return new T[size];
	}

	static void release(T* data, int size) {
		for (int i = 0; i < size; ++i) {
			data[i].resetForReuse();
		}
		pool.push_back({data, size});
	}

private:
	ArrayProvider() = default;

	struct PoolEntry {
		T* data;
		int size;
	};

	static std::vector<PoolEntry> pool;
};

template <typename T>
std::vector<typename ArrayProvider<T>::PoolEntry> ArrayProvider<T>::pool;

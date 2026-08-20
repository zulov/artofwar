#pragma once
#include <algorithm>
#include <vector>

namespace array_provider_detail {
	inline constexpr int MIN_POOL_SIZE = 16;
}

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
		if (size < array_provider_detail::MIN_POOL_SIZE) {
			return new T[size];
		}
		for (auto it = pool.begin(); it != pool.end(); ++it) {
			if (it->size == size) {
				T* data = it->data;
				*it = pool.back();
				pool.pop_back();
				return data;
			}
		}
		return new T[size];
	}

	static void release(T* data, int size) {
		if (data == nullptr) { return; }
		if (size < array_provider_detail::MIN_POOL_SIZE) {
			delete[] data;
			return;
		}
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

template <typename T>
class PrimitiveArrayProvider {
public:
	~PrimitiveArrayProvider() {
		for (auto& entry : pool) {
			delete[] entry.data;
		}
		pool.clear();
	}

	static T* get(int size) {
		return getFromPool(size);
	}

	static T* get(int size, T defaultValue) {
		auto* data = getFromPool(size);
		std::fill_n(data, size, defaultValue);
		return data;
	}

	static void release(T* data, int size) {
		if (data == nullptr) { return; }
		if (size < array_provider_detail::MIN_POOL_SIZE) {
			delete[] data;
			return;
		}
		pool.push_back({data, size});
	}

private:
	static T* getFromPool(int size) {
		if (size < array_provider_detail::MIN_POOL_SIZE) {
			return new T[size];
		}
		for (auto it = pool.begin(); it != pool.end(); ++it) {
			if (it->size == size) {
				T* data = it->data;
				*it = pool.back();
				pool.pop_back();
				return data;
			}
		}
		return new T[size];
	}

	PrimitiveArrayProvider() = default;

	struct PoolEntry {
		T* data;
		int size;
	};

	static std::vector<PoolEntry> pool;
};

template <typename T>
std::vector<typename PrimitiveArrayProvider<T>::PoolEntry> PrimitiveArrayProvider<T>::pool;

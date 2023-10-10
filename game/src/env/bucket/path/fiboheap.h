#pragma once

#include <cassert>
#include <cmath>
#include <queue>
#include <vector>
#include "utils/DeleteUtils.h"

constexpr short CACHE_SIZE = 8192;

static unsigned char DEGREE_CACHE[CACHE_SIZE];

class PriorityHeap {
public:
	struct HeapNode {
		int id;
		float value;
		HeapNode(int _id, float _value) : id(_id), value(_value) {}
		bool operator>(const HeapNode& other) const {
			return value > other.value;
		}
	};

	PriorityHeap() = default;

	PriorityHeap(const PriorityHeap& obj) = delete;


	void clear() {
		pq = std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<>>();
	}

	bool empty() const {
		return pq.empty();
	}

	int get() {
		const auto toReturn = pq.top().id;
		pq.pop();

		return toReturn;
	}

	void put(const int pl, const float k) {
		pq.emplace(pl, k);
	}

private:
	std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<>> pq;
};

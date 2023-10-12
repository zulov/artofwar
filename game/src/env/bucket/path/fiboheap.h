#pragma once

#include <queue>
#include <vector>

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

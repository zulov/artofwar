#pragma once


struct PriorityQueue {
	typedef pair<double, int> PQElement;
	priority_queue<PQElement, vector<PQElement>,
		std::greater<PQElement>> elements;

	inline bool empty() const { return elements.empty(); }

	inline void put(int item, double priority) {
		elements.emplace(priority, item);
	}

	inline int get() {
		int best_item = elements.top().second;
		elements.pop();
		return best_item;
	}
};
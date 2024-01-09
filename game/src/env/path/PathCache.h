#pragma once
#include <vector>

struct PathCache {
	int start = -1;
	int end = -1;
	std::vector<int> path;
	PathCache() = default;
	PathCache(const PathCache& rhs) = delete;

	void clear() {
		start = -1;
		end = -1;
		path.clear();
	}

	bool equal(int _start, int _end) const {
		return start == _start && end == _end;
	}

	void set(int startIdx, int endIdx, const std::vector<int>* vector) {
		start = startIdx;
		end = endIdx;
		path = *vector;
	}
};

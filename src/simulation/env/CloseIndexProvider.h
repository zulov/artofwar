#pragma once
#include <vector>

class CloseIndexProvider {
public:
	explicit CloseIndexProvider(short res);
	std::vector<short>& get(int current);

private:
	short resolution;

	std::vector<short> closeindexes[9];
};

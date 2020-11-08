#pragma once
#include <vector>
class CloseIndexes;

class CloseIndexesProvider {
public:
	~CloseIndexesProvider();

	static CloseIndexes* get(unsigned short resolution);
private:
	CloseIndexesProvider() = default;
	static std::vector<CloseIndexes*> closeIndexeses;
};

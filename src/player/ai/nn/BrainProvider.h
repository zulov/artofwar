#pragma once
#include <string>
#include <vector>

struct Brain;

class BrainProvider {
public:
	~BrainProvider();

	static Brain* get(std::string name);
private:
	BrainProvider() = default;
	static std::vector<Brain*> brains;
	static std::vector<std::string> tempLines;
};

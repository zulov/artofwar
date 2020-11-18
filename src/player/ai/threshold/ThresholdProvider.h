#pragma once
#include <string>
#include <vector>

class Threshold;

class ThresholdProvider {
public:
	~ThresholdProvider();

	static Threshold* get(std::string name);
private:
	ThresholdProvider() = default;
	static std::vector<Threshold*> thresholds;
};

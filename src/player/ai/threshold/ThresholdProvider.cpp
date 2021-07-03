#include "ThresholdProvider.h"

#include "Threshold.h"
#include "utils/DeleteUtils.h"

std::vector<Threshold*> ThresholdProvider::thresholds;

ThresholdProvider::~ThresholdProvider() {
	clear_vector(thresholds);
}

Threshold* ThresholdProvider::get(const std::string name) {
	for (auto* threshold : thresholds) {
		if (threshold->getName() == name) {
			return threshold;
		}
	}
	auto* const threshold = new Threshold(name);
	thresholds.push_back(threshold);
	return threshold;
}

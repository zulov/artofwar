#include "ThresholdProvider.h"

#include <Urho3D/IO/Log.h>

#include "Game.h"
#include "Threshold.h"
#include "utils/DeleteUtils.h"
#include "utils/FileUtils.h"

std::vector<Threshold*> ThresholdProvider::thresholds;
std::vector<std::string> ThresholdProvider::tempLines;

ThresholdProvider::~ThresholdProvider() {
	clear_vector(thresholds);
}

Threshold* ThresholdProvider::get(const std::string name) {
	for (auto* threshold : thresholds) {
		if (threshold->getName() == name) {
			return threshold;
		}
	}
	loadLines(name, tempLines);
	assert(!tempLines.empty());
	if (tempLines.empty()) {
		Game::getLog()->WriteRaw("No threshold Found " + Urho3D::String(name.c_str()) + "\n", true);
		return nullptr;
	}
	auto* const threshold = new Threshold(name, tempLines);
	thresholds.push_back(threshold);
	return threshold;
}

#include "Resources.h"

#include <numeric>

#include "Game.h"
#include "database/DatabaseCache.h"


Resources::Resources() {
	init(0);
}

Resources::Resources(float valueForAll) {
	init(valueForAll);
}

Resources::~Resources() {
	delete[] values;
	delete[] sumGatherSpeed;
	delete[] gatherSpeeds;
};

void Resources::init(float valueForAll) {
	size = Game::getDatabase()->getResourceSize();
	values = new float[size];
	sumGatherSpeed = new float[size];
	gatherSpeeds = new float[size];
	std::fill_n(values, size, valueForAll);
	std::fill_n(gatherSpeeds, size, 0.f);
	std::fill_n(sumGatherSpeed, size, 0.f);

	valuesSpan = std::span(values, size);
	gatherSpeedsSpan = std::span(gatherSpeeds, size);
	sumGatherSpeedSpan = std::span(sumGatherSpeed, size);

	changed = true;
}

bool Resources::reduce(const std::vector<db_cost*>& costs) {
	for (int i = 0; i < costs.size(); ++i) {
		const int j = costs.at(i)->resource;
		values[j] -= costs.at(i)->value;
		if (values[j] < 0) {
			revert(i, costs);
			return false;
		}
	}
	changed = true;
	return true;
}

bool Resources::hasEnough(const std::vector<db_cost*>& costs) const {
	for (auto cost : costs) {
		if (values[cost->resource] < cost->value) {
			return false;
		}
	}
	return true;
}

void Resources::add(int id, float value) {
	values[id] += value;
	sumGatherSpeed[id] += value;
	changed = true;
}

bool Resources::hasChanged() const {
	return changed;
}

std::span<float> Resources::getValues() const {
	return valuesSpan;
}

std::span<float> Resources::getGatherSpeeds() const {
	return gatherSpeedsSpan;
}

void Resources::hasBeenUpdatedDrawn() {
	changed = false;
}

int Resources::getSum() const {
	return std::accumulate(values, values + size, 0);
}

std::string Resources::getValues(int precision, int player) const {
	std::string str;
	for (int i = 0; i < size; ++i) {
		str += "(" + std::to_string(player) + "," + std::to_string(i) + "," + std::to_string(
			(int)(values[i] * precision)) + "),";
	}
	return str;
}

void Resources::setValue(int id, float amount) {
	values[id] = amount;
	changed = true;
}

void Resources::change() {
	changed = true;
}

void Resources::revert(int end, const std::vector<db_cost*>& costs) {
	for (int i = 0; i < end + 1; ++i) {
		const int j = costs.at(i)->resource;
		values[j] += costs.at(i)->value;
	}
}

void Resources::resetStats() const {
	std::copy(sumGatherSpeed, sumGatherSpeed + size, gatherSpeeds);
	std::fill_n(sumGatherSpeed, size, 0.f);
}

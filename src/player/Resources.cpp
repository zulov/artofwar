#include "Resources.h"

#include <numeric>

#include "Game.h"
#include "database/DatabaseCache.h"
#include "math/SpanUtils.h"


Resources::Resources() {
	init(0);
}

Resources::Resources(float valueForAll) {
	init(valueForAll);
}

void Resources::init(float valueForAll) {
	values = std::span(data, RESOURCES_SIZE);
	gatherSpeeds = std::span(data + RESOURCES_SIZE, RESOURCES_SIZE);
	sumGatherSpeed = std::span(data + 2 * RESOURCES_SIZE, RESOURCES_SIZE);
	sumValues = std::span(data + 3 * RESOURCES_SIZE, RESOURCES_SIZE);

	resetSpan(values, valueForAll);
	resetSpan(gatherSpeeds);
	resetSpan(sumGatherSpeed);
	resetSpan(sumValues);

	changed = true;
}

bool Resources::reduce(const std::vector<db_cost*>& costs) {
	if (hasEnough(costs)) {
		for (int i = 0; i < costs.size(); ++i) {
			const int j = costs.at(i)->resource;
			values[j] -= costs.at(i)->value;
		}
		changed = true;
		return true;
	}
	return false;
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
	sumValues[id] += value;
	changed = true;
}

void Resources::hasBeenUpdatedDrawn() {
	changed = false;
}

int Resources::getSum() const {
	return sumSpan(values);
}

std::string Resources::getValues(int precision, int player) const {
	std::string str;
	for (int i = 0; i < RESOURCES_SIZE; ++i) {
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

void Resources::resetStats() const {
	std::copy(sumGatherSpeed.begin(), sumGatherSpeed.end(), gatherSpeeds.begin());
	resetSpan(sumGatherSpeed);
}

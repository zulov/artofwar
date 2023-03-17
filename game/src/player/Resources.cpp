#include "Resources.h"

#include <numeric>

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
}

bool Resources::reduce(const db_cost* costs) {
	if (hasEnough(costs)) {
		for (int i = 0; i < costs->values.size(); ++i) {
			values[i] -= costs->values[i];
		}
		return true;
	}
	return false;
}

bool Resources::hasEnough(const db_cost* costs) const {
	for (int i = 0; i < costs->values.size(); ++i) {
		if (values[i] < costs->values[i]) {
			return false;
		}
	}
	return true;
}

void Resources::add(int id, float value) {
	values[id] += value;
	sumGatherSpeed[id] += value;
	sumValues[id] += value;
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
}

void Resources::resetStats() const {
	std::ranges::copy(sumGatherSpeed, gatherSpeeds.begin());
	resetSpan(sumGatherSpeed);
}

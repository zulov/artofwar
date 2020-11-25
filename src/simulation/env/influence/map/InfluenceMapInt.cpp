#include "InfluenceMapInt.h"
#include <algorithm>
#include <numeric>
#include "objects/Physical.h"

InfluenceMapInt::InfluenceMapInt(unsigned short resolution, float size, float valueThresholdDebug): InfluenceMap(
	resolution, size, valueThresholdDebug) {
	values = new unsigned char[arraySize];
}

InfluenceMapInt::~InfluenceMapInt() {
	delete[] values;
}

void InfluenceMapInt::update(Physical* thing, float value) {
	const int index = calculator->indexFromPosition(thing->getPosition());

	values[index] += round(value);
}

void InfluenceMapInt::updateInt(Physical* thing, int value) {
	values[calculator->indexFromPosition(thing->getPosition())] += value;
}

void InfluenceMapInt::reset() {
	std::fill_n(values, arraySize, 0);
}

char InfluenceMapInt::getValueAt(const Urho3D::Vector2& pos) const {
	auto index = calculator->indexFromPosition(pos);
	return getValueAt(index);
}

float InfluenceMapInt::getValueAsPercent(const Urho3D::Vector2& pos) const {
	const float diff = max - min;
	if (diff != 0.f) {
		return (getValueAt(pos) - min) / diff;
	}
	return 0.5f;
}

float InfluenceMapInt::getValueAsPercent(const int index) const {
	const float diff = max - min;
	if (diff != 0.f) {
		return (getValueAt(index) - min) / diff;
	}
	return 0.5f;
}

float InfluenceMapInt::getValueAt(int index) const {
	return values[index];
}

void InfluenceMapInt::finishCalc() {
	const auto [minIdx, maxIdx] = std::minmax_element(values, values + arraySize);
	min = *minIdx;
	max = *maxIdx;
	avg = std::accumulate(values, values + arraySize, 0) / (double)arraySize;
}

int InfluenceMapInt::getMaxElement() {
	return std::distance(values, std::max_element(values, values + arraySize));
}

int InfluenceMapInt::getMaxElement(const std::array<int, 4>& indexes) {
	assert(indexes[0]<arraySize && indexes[1]<arraySize &&indexes[2]<arraySize &&indexes[3]<arraySize);
	float values1[4] = {values[indexes[0]], values[indexes[1]], values[indexes[2]], values[indexes[3]]};
	int i = std::distance(values1, std::max_element(values1, values1 + 4));
	return indexes[i];
}

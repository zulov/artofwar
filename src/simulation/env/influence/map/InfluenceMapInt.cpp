#include "InfluenceMapInt.h"
#include <algorithm>
#include <numeric>
#include "objects/Physical.h"

InfluenceMapInt::InfluenceMapInt(unsigned short resolution, float size, float valueThresholdDebug): InfluenceMap(
	resolution, size, valueThresholdDebug) {
	values = new unsigned char[arraySize];
	reset();
}

InfluenceMapInt::~InfluenceMapInt() {
	delete[] values;
}

void InfluenceMapInt::update(Physical* thing, float value) {
	const int index = calculator->indexFromPosition(thing->getPosition());

	values[index] += round(value);
}

void InfluenceMapInt::updateInt(Physical* thing, int value) {
	updateInt(calculator->indexFromPosition(thing->getPosition()), value);
}

void InfluenceMapInt::updateInt(int index, int value) const {
	values[index] += value;
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

void InfluenceMapInt::computeMinMax() {
	if (!minMaxInited) {
		const auto [minIdx, maxIdx] = std::minmax_element(values, values + arraySize);
		min = *minIdx;
		max = *maxIdx;
		minMaxInited = true;
	}
}

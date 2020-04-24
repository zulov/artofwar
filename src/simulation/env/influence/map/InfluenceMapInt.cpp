#include "InfluenceMapInt.h"
#include "objects/Physical.h"
#include <algorithm>
#include <numeric>

InfluenceMapInt::InfluenceMapInt(unsigned short resolution, float size, float valueThresholdDebug): InfluenceMap(
	resolution, size, valueThresholdDebug) {
	values = new unsigned char[arraySize];
}

InfluenceMapInt::~InfluenceMapInt() {
	delete[] values;
}

void InfluenceMapInt::update(Physical* physical, float value) {
	const int index = calculator.indexFromPosition(physical->getPosition());

	values[index] += round(value);
}

void InfluenceMapInt::reset() {
	std::fill_n(values, arraySize, 0);
}

char InfluenceMapInt::getValueAt(const Urho3D::Vector2& pos) const {
	auto index = calculator.indexFromPosition(pos);
	return getValueAt(index);
}

float InfluenceMapInt::getValueAsPercent(const Urho3D::Vector2& pos) const {
	const float diff = max - min;
	if (diff != 0) {
		return (getValueAt(pos) - min) / diff;
	}
	return 0.5;
}

float InfluenceMapInt::getValueAt(int index) const {
	return values[index];
}

void InfluenceMapInt::calcStats() {
	const auto [minIdx, maxIdx] = std::minmax_element(values, values + arraySize);
	min = *minIdx;
	max = *maxIdx;
	avg = std::accumulate(values, values + arraySize, 0) / (double)arraySize;
}

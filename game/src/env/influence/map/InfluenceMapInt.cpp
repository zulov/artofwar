#include "InfluenceMapInt.h"
#include <algorithm>
#include <cassert>
#include <numeric>

#include "math/VectorUtils.h"

InfluenceMapInt::InfluenceMapInt(unsigned short resolution, float size, float valueThresholdDebug): InfluenceMap(
	 resolution, size, valueThresholdDebug) {
	values = new unsigned char[arraySize];
	reset();
}

InfluenceMapInt::~InfluenceMapInt() {
	delete[] values;
}

void InfluenceMapInt::update(unsigned index, unsigned char value) const {
	assert(values[index] + value >= values[index] && "unsigned char overflow in InfluenceMapInt::updateInt");
	values[index] += value;
}

void InfluenceMapInt::reset() {
	std::fill_n(values, arraySize, 0);
	minMaxInited = false;
}

unsigned char InfluenceMapInt::getValueAt(const Urho3D::Vector2& pos) const {
	auto index = calculator->indexFromPosition(pos);
	return getValueAt(index);
}

float InfluenceMapInt::getValueAt(unsigned index) const {
	return values[index];
}

void InfluenceMapInt::ensureReady() {
	if (!minMaxInited) {
		const auto [minPtr, maxPtr] = std::minmax_element(values, values + arraySize);
		min = *minPtr;
		max = *maxPtr;
		minIdx = std::distance(values, minPtr);
		maxIdx = std::distance(values, maxPtr);
		minMaxInited = true;
	}
}

std::vector<unsigned> InfluenceMapInt::getMaxIdxs() {
	ensureReady();
	if (max <= 0.5f) {
		return {};
	}
	auto idx = sort_indexes_desc(std::span(values, arraySize), 10);

	for (int i = 0; i < 10; ++i) {
		if (values[idx[i]] == 0) {
			const auto it = idx.begin() + i;
			idx.erase(it, idx.end());
			break;
		}
	}
	return idx;
}

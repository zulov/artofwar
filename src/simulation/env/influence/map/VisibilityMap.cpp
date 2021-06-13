#include "VisibilityMap.h"

#include "objects/Physical.h"

VisibilityMap::VisibilityMap(unsigned short resolution, float size, float valueThresholdDebug): InfluenceMap(
	resolution, size, valueThresholdDebug) {
	values = new char[arraySize];
	std::fill_n(values, arraySize, -1);
}

VisibilityMap::~VisibilityMap() {
	delete[] values;
}

void VisibilityMap::update(Physical* thing, float value) {
	char sRadius = thing->getSightRadius();
	if (sRadius < 0) { return; }
	//TODO nie kwadrat tylko ko³os
	auto index = calculator->indexFromPosition(thing->getPosition());
	auto [centerX,centerZ] = calculator->getIndexes(index);
	const auto minI = calculator->getValid(centerX - sRadius);
	const auto maxI = calculator->getValid(centerX + sRadius);

	const auto minJ = calculator->getValid(centerZ - sRadius);
	const auto maxJ = calculator->getValid(centerZ + sRadius);


	for (short i = minI; i <= maxI; ++i) {
		const int index = calculator->getNotSafeIndex(i, minJ);
		auto* t = &values[index];
		for (short j = minJ; j <= maxJ; ++j) {
			*(t++) = 1;
		}
	}
}

void VisibilityMap::updateInt(Physical* thing, int value) {
	update(thing);
}

void VisibilityMap::updateInt(int index, int value) const {
	assert(false);
}


void VisibilityMap::reset() {
	for (int i = 0; i < arraySize; ++i) {
		if (values[i] == 1) {
			values[i] = 0;
		}
	}
}

char VisibilityMap::getValueAt(const Urho3D::Vector2& pos) const {
	auto index = calculator->indexFromPosition(pos);
	return getValueAt(index);
}

float VisibilityMap::getValueAsPercent(const Urho3D::Vector2& pos) const {
	const float diff = max - min;
	if (diff != 0.f) {
		return (getValueAt(pos) - min) / diff;
	}
	return 0.5f;
}

float VisibilityMap::getValueAsPercent(const int index) const {
	const float diff = max - min;
	if (diff != 0.f) {
		return (getValueAt(index) - min) / diff;
	}
	return 0.5f;
}

float VisibilityMap::getValueAt(int index) const {
	return values[index];
}

void VisibilityMap::computeMinMax() {
	if (!minMaxInited) {
		const auto [minIdx, maxIdx] = std::minmax_element(values, values + arraySize);
		min = *minIdx;
		max = *maxIdx;
		minMaxInited = true;
	}
}

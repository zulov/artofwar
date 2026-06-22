#include "VisibilityMap.h"
#include <cassert>

#include "VisibilityType.h"
#include "math/MathUtils.h"
#include "utils/SpanUtils.h"
#include "objects/Physical.h"
#include "env/bucket/levels/LevelCache.h"
#include "env/bucket/levels/LevelCacheProvider.h"
#include "env/influence/VisibilityManager.h"
#include "utils/OtherUtils.h"


VisibilityMap::VisibilityMap(unsigned short resolution, float size, float valueThresholdDebug) :
	InfluenceMap(resolution, size, valueThresholdDebug),
	influenceRes(resolution / 2),
	influenceArraySize(influenceRes * influenceRes),
	levelCache(LevelCacheProvider::get(resolution, 60.f, calculator)) {
	values = new VisibilityType[arraySize];
	std::fill_n(values, arraySize, VisibilityType::NONE);
	valuesForInfluence = new bool[influenceArraySize];
	std::fill_n(valuesForInfluence, influenceArraySize, false);
	ranges = new float[arraySize];
	std::fill_n(ranges, arraySize, 0.f);
}

VisibilityMap::~VisibilityMap() {
	delete[] values;
	delete[] valuesForInfluence;
	delete[] ranges;
}

void VisibilityMap::update(const Urho3D::Vector2& pos, float sRadius) {
	if (sRadius < 0) { return; }
	valuesForInfluenceReady = false;
	percentReady = false;
	const auto centerIdx = calculator->indexFromPosition(pos);
	if (ranges[centerIdx] < sRadius) {
		if (ranges[centerIdx] == 0.f && changedIndexes.size() < CHANGED_INDEXES_MAX_SIZE) {
			changedIndexes.push_back(centerIdx);
		}
		ranges[centerIdx] = sRadius;
	}
}

void VisibilityMap::finishAtIndex(unsigned i) const {
	const auto levels = levelCache->get(ranges[i], i);

	for (const auto idx : *levels) {
		assert(i + idx >= 0 && i + idx < arraySize && "out-of-bounds in VisibilityMap::finishAtIndex");
		values[i + idx] = VisibilityType::VISIBLE;
	}

	ranges[i] = 0.f;
}

void VisibilityMap::finish() {
	if (changedIndexes.size() >= CHANGED_INDEXES_MAX_SIZE) {
		for (unsigned i = 0; i < arraySize; ++i) { if (ranges[i] > 0.f) { finishAtIndex(i); } }
	} else { for (const int i : changedIndexes) { finishAtIndex(i); } }

	changedIndexes.clear();
}

void VisibilityMap::reset() {
	valuesForInfluenceReady = false;
	percentReady = false;
	minMaxInited = false;
	char* end = (char*)values + arraySize;
	for (char* i = (char*)values; i < end; i++) { *i &= 1; }
}

char VisibilityMap::getValueAt(const Urho3D::Vector2& pos) const {
	return getValueAt(calculator->indexFromPosition(pos));
}

VisibilityType VisibilityMap::getValueAt(float x, float z) const {
	char val = getValueAt(calculator->indexFromPosition(x, z));
	return static_cast<VisibilityType>(val);
}

float VisibilityMap::getValueAsPercent(const Urho3D::Vector2& pos) const {
	const float diff = max - min;
	if (diff != 0.f) { return (getValueAt(pos) - min) / diff; }
	return 0.5f;
}

float VisibilityMap::getValueAsPercent(const int index) const {
	const float diff = max - min;
	if (diff != 0.f) { return (getValueAt(index) - min) / diff; }
	return 0.5f;
}

float VisibilityMap::getValueAt(int index) const {
	assert(index < getResolution() * getResolution());
	return castC(values[index]);
}

void VisibilityMap::computeMinMax() {
	if (!minMaxInited) {
		const auto [minPtr, maxPtr] = std::minmax_element(values, values + arraySize);
		min = castC(*minPtr);
		max = castC(*maxPtr);
		minIdx = std::distance(values, minPtr);
		maxIdx = std::distance(values, maxPtr);
		minMaxInited = true;
	}
}

int VisibilityMap::removeUnseen(float* intersection) {
	ensureReady();
	int notSeen = 0;
	const auto end = valuesForInfluence + influenceArraySize;

	for (auto ptrI = valuesForInfluence; ptrI < end; ++ptrI, ++intersection) {
		if (!(*ptrI)) {
			++notSeen;
			*intersection = std::numeric_limits<float>::max();
		} else { *intersection = 0.f; }
	}
	return influenceArraySize - notSeen;
}

float VisibilityMap::getPercent() {
	if (!percentReady) {
		const int sum = std::accumulate((char*)values, (char*)(values + arraySize), 0);
		percent = sum / (arraySize * 3.f);
		percentReady = true;
	}
	return percent;
}

void VisibilityMap::ensureReady() {
	if (valuesForInfluenceReady == false) {
		std::fill_n(valuesForInfluence, influenceArraySize, false);
		const auto parent = values;
		const auto current = valuesForInfluence;
		int j = 0;
		auto res = getResolution();
		for (int prow = 0; prow < res; ++prow) {
			const int rowBase = (prow / 2) * influenceRes;
			for (int pcol = 0; pcol < res; ++pcol, ++j) {
				if (parent[j] == VisibilityType::VISIBLE) {
					const int newIndex = rowBase + (pcol / 2);
					assert(newIndex < influenceRes * influenceRes);
					current[newIndex] = true;
				}
			}
		}
		valuesForInfluenceReady = true;
	}
}

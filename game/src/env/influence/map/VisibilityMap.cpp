#include "VisibilityMap.h"

#include "VisibilityType.h"
#include "math/MathUtils.h"
#include "math/SpanUtils.h"
#include "objects/Physical.h"
#include "env/bucket/levels/LevelCache.h"
#include "env/bucket/levels/LevelCacheProvider.h"
#include "env/influence/VisibilityManager.h"
#include "utils/OtherUtils.h"


VisibilityMap::VisibilityMap(unsigned short resolution, float size, float valueThresholdDebug)
	: InfluenceMap(resolution, size, valueThresholdDebug),
	  levelCache(LevelCacheProvider::get(resolution, true, 60.f, calculator)) {
	values = new VisibilityType[arraySize];
	std::fill_n(values, arraySize, VisibilityType::NONE);
	valuesForInfluence = new bool[arraySize / 4];
	std::fill_n(valuesForInfluence, arraySize / 4, false);
	ranges = new float[arraySize];
	std::fill_n(ranges, arraySize, 0.f);
}

VisibilityMap::~VisibilityMap() {
	delete[] values;
	delete[] valuesForInfluence;
	delete[] ranges;
}

void VisibilityMap::update(Physical* thing, float value) {
	valuesForInfluenceReady = false;
	percentReady = false;
	const auto sRadius = thing->getSightRadius();
	if (sRadius < 0) { return; }
	const auto centerIdx = calculator->indexFromPosition(thing->getPosition());
	if (ranges[centerIdx] < sRadius) {
		if (ranges[centerIdx] == 0.f && changedIndexes.size() < CHANGED_INDEXES_MAX_SIZE) {
			changedIndexes.push_back(centerIdx);
		}
		ranges[centerIdx] = sRadius;
	}
}

void VisibilityMap::finishAtIndex(int i) const {
	const auto centerCords = calculator->getIndexes(i);
	for (const auto& [idx, shift] : levelCache->getBoth(ranges[i])) {
		if (calculator->isValidIndex(centerCords + shift)) {
			const auto index = i + idx;
			values[index] = VisibilityType::VISIBLE;
		}
	}
	ranges[i] = 0.f;
}

void VisibilityMap::finish() {
	if (changedIndexes.size() >= CHANGED_INDEXES_MAX_SIZE) {
		for (int i = 0; i < arraySize; ++i) {
			if (ranges[i] > 0.f) {
				finishAtIndex(i);
			}
		}
	} else {
		for (const int i : changedIndexes) {
			finishAtIndex(i);
		}
	}

	changedIndexes.clear();
}

void VisibilityMap::updateInt(Physical* thing, int value) {
	update(thing);
}

void VisibilityMap::updateInt(int index, int value) const {
	assert(false);
}

void VisibilityMap::reset() {
	valuesForInfluenceReady = false;
	percentReady = false;
	minMaxInited = false;
	char* end = (char*)values + arraySize;
	for (char* i = (char*)values; i < end; i++) {
		*i &= 1;
	}
	// auto func = [](VisibilityType vt) -> float { return vt == VisibilityType::VISIBLE; };
	// std::replace_if(values, values + arraySize, func, VisibilityType::SEEN);
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
	assert(index < getResolution() * getResolution());
	return cast(values[index]);
}

void VisibilityMap::computeMinMax() {
	if (!minMaxInited) {
		const auto [minPtr, maxPtr] = std::minmax_element(values, values + arraySize);
		min = cast(*minPtr);
		max = cast(*maxPtr);
		minIdx = std::distance(values, minPtr);
		maxIdx = std::distance(values, maxPtr);
		minMaxInited = true;
	}
}

int VisibilityMap::removeUnseen(float* intersection) {
	ensureReady();
	int notSeen = 0;
	const auto end = valuesForInfluence + arraySize / 4;
	for (auto ptrI = valuesForInfluence; ptrI < end; ++ptrI, ++intersection) {
		if (!(*ptrI)) {
			++notSeen;
			*intersection = std::numeric_limits<float>::max();
		}
	}
	return arraySize / 4 - notSeen;
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
		std::fill_n(valuesForInfluence, arraySize / 4, false);
		const auto parent = values;
		const int parentRes = sqrt(arraySize); //TODO bug a co z zaokragleniem
		const auto current = valuesForInfluence;
		const int currentRes = sqrt(arraySize / 4);
		for (int j = 0; j < arraySize; ++j) {
			if (parent[j] == VisibilityType::VISIBLE) {
				const int newIndex = getCordsInLower(currentRes, parentRes, j);
				assert(newIndex<currentRes*currentRes);
				current[newIndex] = true;
			}
		}
		valuesForInfluenceReady = true;
	}
}

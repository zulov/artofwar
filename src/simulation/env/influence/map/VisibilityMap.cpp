#include "VisibilityMap.h"

#include "VisibilityType.h"
#include "math/SpanUtils.h"
#include "objects/Physical.h"
#include "simulation/env/GridCalculatorProvider.h"
#include "simulation/env/bucket/levels/LevelCache.h"
#include "simulation/env/bucket/levels/LevelCacheProvider.h"
#include "utils/OtherUtils.h"

VisibilityMap::VisibilityMap(unsigned short resolution, float size, float valueThresholdDebug)
	: InfluenceMap(resolution, size, valueThresholdDebug),
	  levelCache(LevelCacheProvider::get(resolution, false, 60.f, calculator)) {
	values = new VisibilityType[arraySize];
	std::fill_n(values, arraySize, VisibilityType::NONE);
	valuesForInfluence = new bool[arraySize / 4];
	std::fill_n(valuesForInfluence, arraySize / 4, false);
}

VisibilityMap::~VisibilityMap() {
	delete[] values;
	delete[] valuesForInfluence;
}

void VisibilityMap::update(Physical* thing, float value) {
	valuesForInfluenceReady = false;
	percentReady = false;
	const auto sRadius = thing->getSightRadius();
	if (sRadius < 0) { return; }
	const auto centerIdx = calculator->indexFromPosition(thing->getPosition());
	for (auto level : *levelCache->get(sRadius)) {
		const auto index = centerIdx + level; //TOdo to trzeba inaczej sprawdzic, bo to wychodzi poza plansze i tak
		if (calculator->isValidIndex(index)) {
			values[index] = VisibilityType::VISIBLE;
		}
	}
}

void VisibilityMap::updateInt(Physical* thing, int value) {
	valuesForInfluenceReady = false;
	percentReady = false;
	update(thing);
}

void VisibilityMap::updateInt(int index, int value) const {
	assert(false);
}

void VisibilityMap::reset() {
	valuesForInfluenceReady = false;
	percentReady = false;
	auto end = values + arraySize;
	for (auto i = values; i < end; ++i) {
		if (*i == VisibilityType::VISIBLE) {
			*i = VisibilityType::SEEN;
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
	return cast(values[index]);
}

void VisibilityMap::computeMinMax() {
	if (!minMaxInited) {
		const auto [minIdx, maxIdx] = std::minmax_element(values, values + arraySize);
		min = cast(*minIdx);
		max = cast(*maxIdx);
		minMaxInited = true;
	}
}

void VisibilityMap::removeUnseen(float* intersection) {
	ensureReady();
	const auto  end = valuesForInfluence + arraySize / 4;
	for (auto ptrI = valuesForInfluence; ptrI < end; ++ptrI, ++intersection) {
		if (!(*ptrI)) {
			*intersection = std::numeric_limits<float>::max();
		}
	}
}

float VisibilityMap::getPercent() {
	if (!percentReady) {
		float sum = std::accumulate((char*)values, (char*)(values + arraySize), 0.f);
		percent = sum / (arraySize * 2.f);
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
				auto x = j / parentRes;
				auto z = j % parentRes;
				x /= 2;
				z /= 2;

				const int newIndex = x * currentRes + z;
				assert(newIndex<currentRes*currentRes);
				current[newIndex] = true;
			}
		}
		valuesForInfluenceReady = true;
	}
}

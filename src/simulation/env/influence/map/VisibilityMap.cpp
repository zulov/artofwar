#include "VisibilityMap.h"

#include "VisibilityType.h"
#include "objects/Physical.h"
#include "simulation/env/GridCalculatorProvider.h"
#include "simulation/env/bucket/levels/LevelCache.h"
#include "simulation/env/bucket/levels/LevelCacheProvider.h"
#include "utils/OtherUtils.h"

VisibilityMap::VisibilityMap(unsigned short resolution, float size, float valueThresholdDebug)
	: InfluenceMap(resolution, size, valueThresholdDebug),
	  levelCache(LevelCacheProvider::get(resolution, 60.f, calculator)) {
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
	auto sRadius = thing->getSightRadius();
	if (sRadius < 0) { return; }
	auto centerIdx = calculator->indexFromPosition(thing->getPosition());
	auto levels = levelCache->get(sRadius);
	for (auto level : *levels) {
		auto index = centerIdx + level;
		if (calculator->isValidIndex(index)) {
			values[index] = VisibilityType::VISIBLE;
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
		if (values[i] == VisibilityType::VISIBLE) {
			values[i] = VisibilityType::SEEN;
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

void VisibilityMap::ensureReady() {
	if (valuesForInfluenceReady == false) {
		std::fill_n(valuesForInfluence, arraySize / 4, false);


		auto parent = values;
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

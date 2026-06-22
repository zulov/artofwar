#include "InfluenceMapFloat.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include "math/MathUtils.h"
#include "objects/Physical.h"
#include "env/Environment.h"
#include "env/influence/VisibilityManager.h"

InfluenceMapFloat::
InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug,
                  float* sharedTemplateV) :
	InfluenceMap(resolution, size, valueThresholdDebug),
	coef(coef),
	level(level) {
	levelRes = level * 2 + 1;

	values = new float[arraySize];
	tempVals = new float[arraySize];
	templateV = sharedTemplateV;

	std::fill_n(values, arraySize, 0.f);
	std::fill_n(tempVals, arraySize, 0.f);

	assert(level > 0);
}

float* InfluenceMapFloat::createTemplateV(float coef, char level) {
	const auto levelRes = level * 2 + 1;
	auto* tv = new float[levelRes * levelRes];
	auto* ptr = tv;
	for (short i = -level; i <= level; ++i) {
		const auto a = i * i;
		for (short j = -level; j <= level; ++j) {
			const auto b = j * j;
			*(ptr++) = 1 / ((a + b) * coef + 1.f);
		}
	}
	return tv;
}

InfluenceMapFloat::~InfluenceMapFloat() {
	delete[] values;
	delete[] tempVals;
}

void InfluenceMapFloat::tempUpdate(const Urho3D::Vector2& pos, float value) {
	tempUpdate(calculator->indexFromPosition(pos), value);
}

void InfluenceMapFloat::tempUpdate(int index, float value) {
	if (tempVals[index] == 0.f && changedIndexes.size() < CHANGED_INDEXES_MAX_SIZE) { changedIndexes.push_back(index); }
	tempVals[index] += value;
	valuesCalculateNeeded = true;
}

void InfluenceMapFloat::applyKernel(int index) const {
	auto [centerX, centerZ] = calculator->getCords(index);
	auto value = tempVals[index];
	const auto minI = calculator->getValidLow(centerX - level);
	const auto maxI = calculator->getValidHigh(centerX + level);

	const auto minJ = calculator->getValidLow(centerZ - level);
	const auto maxJ = calculator->getValidHigh(centerZ + level);

	const auto jStart = (minJ - centerZ + level);
	for (auto i = minI; i <= maxI; ++i) {
		auto* t = &values[calculator->getNotSafeIndex(i, minJ)];
		auto idx = (i - centerX + level) * levelRes + jStart;
		auto ptr = templateV + idx;
		for (short j = minJ; j <= maxJ; ++j) {
			*(t++) += value * *(ptr++);
		}
	}
	//TODO to chyba jest błedne miejsce
	tempVals[index] = 0.f;
}

void InfluenceMapFloat::reset() {
	minMaxInited = false;
	std::fill_n(values, arraySize, 0.f);
}

float InfluenceMapFloat::getValueAt(unsigned index) const {
	assert(!valuesCalculateNeeded);
	return values[index];
}

float InfluenceMapFloat::getValueAsPercent(unsigned index) const {
	assert(minMaxInited);
	assert(!valuesCalculateNeeded);
	const float diff = max - min;
	if (diff != 0.f) { return (getValueAt(index) - min) / diff; }
	return 0.5f;
}

void InfluenceMapFloat::computeMinMax() {
	assert(!valuesCalculateNeeded);
	if (!minMaxInited) {
		const auto [minPtr, maxPtr] = std::minmax_element(values, values + arraySize);
		min = *minPtr;
		max = *maxPtr;
		minIdx = std::distance(values, minPtr);
		maxIdx = std::distance(values, maxPtr);
		minMaxInited = true;
	}
}

void InfluenceMapFloat::ensureReady() {
	updateFromTemp();
	computeMinMax();
}

std::vector<int> InfluenceMapFloat::getIndexesWithByValue(float percent, float tolerance) {
	const float diff = max - min;
	auto minV = diff * (percent - tolerance) + min;
	auto maxV = diff * (percent + tolerance) + min;

	float* i = values;
	std::vector<int> indexes;
	indexes.reserve(16);
	auto pred = [minV,maxV](float v) { return v >= minV && v <= maxV; };
	while ((i = std::find_if(i, values + arraySize, pred)) != values + arraySize) {
		//TODO performance!!!	
		indexes.push_back(i - values);
		++i;
	}
	return indexes; //TODO should stay sorted
}

bool InfluenceMapFloat::cumulateErrors(float percent, float* intersection) {
	ensureReady();
	assert(minMaxInited);
	assert(!valuesCalculateNeeded);

	const float diff = max - min;

	if (diff != 0.f) {
		//const auto centerVal = percent * diff + min;
		const auto coef = 1.f / diff * percent;
		const auto endV = values + arraySize;
		if (percent < 0.f) {
			for (auto ptrV = values; ptrV < endV; ++ptrV, ++intersection) {
				float val = (*ptrV - min) * coef;
				*intersection += val * val;
			}
		} else {
			for (auto ptrV = values; ptrV < endV; ++ptrV, ++intersection) {
				float val = (max - *ptrV) * coef;
				*intersection += val * val;
			}
		}
		return true;
	}
	return false;
}

void InfluenceMapFloat::updateFromTemp() {
	if (valuesCalculateNeeded) {
		if (changedIndexes.size() >= CHANGED_INDEXES_MAX_SIZE) {
			for (int i = 0; i < arraySize; ++i) {
				const auto val = tempVals[i];
				if (val > 0.f) { applyKernel(i); }
			}
		} else { for (const int i : changedIndexes) { applyKernel(i); } }
		changedIndexes.clear();
		//std::fill_n(tempVals, arraySize, 0.f);
		valuesCalculateNeeded = false;
	}
}

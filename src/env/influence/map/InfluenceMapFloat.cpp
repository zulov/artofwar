#include "InfluenceMapFloat.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include "math/MathUtils.h"
#include "objects/Physical.h"
#include "env/Environment.h"

InfluenceMapFloat::
InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug):
	InfluenceMap(resolution, size, valueThresholdDebug),
	coef(coef),
	level(level) {
	levelRes = level * 2 + 1;

	values = new float[arraySize];
	tempVals = new float[arraySize];
	templateV = new float[levelRes * levelRes];

	std::fill_n(values, arraySize, 0.f);
	std::fill_n(tempVals, arraySize, 0.f);


	auto* ptr = templateV;
	for (short i = -level; i <= level; ++i) {
		const auto a = i * i;
		for (short j = -level; j <= level; ++j) {
			const auto b = j * j;
			*(ptr++) = 1 / ((a + b) * coef + 1.f);
		}
	}

	assert(level > 0);
}

InfluenceMapFloat::~InfluenceMapFloat() {
	delete[] values;
	delete[] tempVals;
	delete[] templateV;
}

void InfluenceMapFloat::update(Physical* thing, float value) {
	assert(false);
	auto& pos = thing->getPosition();
	update(value, calculator->getIndex(pos.x_), calculator->getIndex(pos.z_));
}

void InfluenceMapFloat::updateInt(Physical* thing, int value) {
	assert(false);
}

void InfluenceMapFloat::tempUpdate(const Urho3D::Vector3& pos, float value) {
	tempUpdate(calculator->indexFromPosition(pos), value);
}

void InfluenceMapFloat::tempUpdate(int index, float value) {
	tempVals[index] += value;
	valuesCalculateNeeded = true;
}

void InfluenceMapFloat::update(int index, float value) const {
	auto [centerX,centerZ] = calculator->getIndexes(index);
	update(value, centerX, centerZ);
}

void InfluenceMapFloat::update(float value, const unsigned short centerX, const unsigned short centerZ) const {
	const auto minI = calculator->getValidLow(centerX - level);
	const auto maxI = calculator->getValidHigh(centerX + level);

	const auto minJ = calculator->getValidLow(centerZ - level);
	const auto maxJ = calculator->getValidHigh(centerZ + level);

	const auto jStart = (minJ - centerZ + level);
	for (short i = minI; i <= maxI; ++i) {
		const int index = calculator->getNotSafeIndex(i, minJ);
		auto* t = &values[index];
		auto idx = (i - centerX + level) * levelRes + jStart;
		auto ptr = templateV + idx;
		for (short j = minJ; j <= maxJ; ++j) {
			*(t++) += value * *(ptr++);
		}
	}
}

void InfluenceMapFloat::reset() {
	minMaxInited = false;
	std::fill_n(values, arraySize, 0.f);
}

float InfluenceMapFloat::getValueAt(int index) const {
	assert(!valuesCalculateNeeded);
	return values[index];
}

float InfluenceMapFloat::getValueAt(const Urho3D::Vector2& pos) const {
	assert(!valuesCalculateNeeded);
	return getValueAt(calculator->indexFromPosition(pos));
}

float InfluenceMapFloat::getValueAsPercent(const Urho3D::Vector2& pos) const {
	assert(minMaxInited);
	assert(!valuesCalculateNeeded);
	const float diff = max - min;
	if (diff != 0.f) {
		return (getValueAt(pos) - min) / diff;
	}
	return 0.5f;
}

float InfluenceMapFloat::getValueAsPercent(int index) const {
	assert(minMaxInited);
	assert(!valuesCalculateNeeded);
	const float diff = max - min;
	if (diff != 0.f) {
		return (getValueAt(index) - min) / diff;
	}
	return 0.5f;
}

void InfluenceMapFloat::computeMinMax() {
	assert(!valuesCalculateNeeded);
	if (!minMaxInited) {
		const auto [minIdx, maxIdx] = std::minmax_element(values, values + arraySize);
		min = *minIdx;
		max = *maxIdx;
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

	float* iter = values;
	std::vector<int> indexes;
	indexes.reserve(16);
	auto pred = [minV,maxV](float i) { return i >= minV && i <= maxV; };
	while ((iter = std::find_if(iter, values + arraySize, pred)) != values + arraySize) {
		//TODO performance!!!	
		indexes.push_back(iter - values);
		++iter;
	}
	return indexes; //TODO should stay sorted
}

bool InfluenceMapFloat::cumulateErros(float percent, float* intersection) const {
	assert(minMaxInited);
	assert(!valuesCalculateNeeded);
	float diff = max - min;

	if (diff != 0.f) {
		diff = 1.f / diff;
		percent = fixValue(percent, 1.f);
		const auto centerVal = percent * diff + min;
		const auto endV = values + arraySize;
		const auto max =std::numeric_limits<float>::max() - 1.f;

		for (auto ptrV = values; ptrV < endV; ++ptrV, ++intersection) {
			if (*intersection < max) {
				const auto val = (centerVal - (*ptrV)) * diff;
				*intersection += val * val;
			}
		}
		return true;
	}
	return false;
}

void InfluenceMapFloat::updateFromTemp() {
	if (valuesCalculateNeeded) {
		for (int i = 0; i < arraySize; ++i) {
			const auto val = tempVals[i];
			if (val > 0.f) {
				update(i, val);
			}
		}
		std::fill_n(tempVals, arraySize, 0.f);
		valuesCalculateNeeded = false;
	}
}

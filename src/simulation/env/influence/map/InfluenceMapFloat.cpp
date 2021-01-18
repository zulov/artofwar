#include "InfluenceMapFloat.h"
#include <algorithm>
#include <numeric>
#include "math/MathUtils.h"
#include "objects/Physical.h"
#include "simulation/env/Environment.h"

InfluenceMapFloat::
InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug):
	InfluenceMap(resolution, size, valueThresholdDebug),
	coef(coef),
	level(level) {
	values = new float[arraySize];
	tempVals = new float[arraySize];
}

InfluenceMapFloat::~InfluenceMapFloat() {
	delete[] values;
	delete[] tempVals;
}

void InfluenceMapFloat::update(Physical* thing, float value) {
	auto& pos = thing->getPosition();

	update(value, calculator->getIndex(pos.x_), calculator->getIndex(pos.z_));
}

void InfluenceMapFloat::updateInt(Physical* thing, int value) {
	assert(false);
}

void InfluenceMapFloat::tempUpdate(Physical* thing, float value) {
	tempUpdate(calculator->indexFromPosition(thing->getPosition()), value);
}

void InfluenceMapFloat::tempUpdate(int index, float value) {
	tempVals[index] += value;
	tempComputedNeeded = true;
}

void InfluenceMapFloat::update(int index, float value) const {
	auto [centerX,centerZ] = calculator->getIndexes(index);
	update(value, centerX, centerZ);
}

void InfluenceMapFloat::update(float value, const unsigned short centerX, const unsigned short centerZ) const {
	const auto minI = calculator->getValid(centerX - level);
	const auto maxI = calculator->getValid(centerX + level);

	const auto minJ = calculator->getValid(centerZ - level);
	const auto maxJ = calculator->getValid(centerZ + level);

	for (short i = minI; i <= maxI; ++i) {
		const auto a = (i - centerX) * (i - centerX);
		const int index = calculator->getNotSafeIndex(i, minJ);
		auto* t = &values[index];
		for (short j = minJ; j <= maxJ; ++j) {
			const auto b = (j - centerZ) * (j - centerZ);

			*(t++) += value / ((a + b) * coef + 1.f);
		}
	}
}

void InfluenceMapFloat::reset() {
	std::fill_n(values, arraySize, 0.f);
}

float InfluenceMapFloat::getValueAt(int index) const {
	return values[index];
}

float InfluenceMapFloat::getValueAt(const Urho3D::Vector2& pos) const {
	auto index = calculator->indexFromPosition(pos);
	return getValueAt(index);
}

float InfluenceMapFloat::getValueAsPercent(const Urho3D::Vector2& pos) const {
	assert(minMaxInited);
	const float diff = max - min;
	if (diff != 0.f) {
		return (getValueAt(pos) - min) / diff;
	}
	return 0.5f;
}

float InfluenceMapFloat::getValueAsPercent(int index) const {
	assert(minMaxInited);
	const float diff = max - min;
	if (diff != 0.f) {
		return (getValueAt(index) - min) / diff;
	}
	return 0.5f;
}

void InfluenceMapFloat::computeMinMax() {
	if (!minMaxInited) {
		const auto [minIdx, maxIdx] = std::minmax_element(values, values + arraySize);
		min = *minIdx;
		max = *maxIdx;
		minMaxInited = true;
	}
}

int InfluenceMapFloat::getMaxElement() {
	return std::distance(values, std::max_element(values, values + arraySize));
}

int InfluenceMapFloat::getMaxElement(const std::array<int, 4>& indexes) {
	float values1[4] = {values[indexes[0]], values[indexes[1]], values[indexes[2]], values[indexes[3]]};
	int i = std::distance(values1, std::max_element(values1, values1 + 4));
	return indexes[i];
}

std::vector<int> InfluenceMapFloat::getIndexesWithByValue(float percent, float tolerance) const {
	const float diff = max - min;
	auto minV = diff * (percent - tolerance) + min;
	auto maxV = diff * (percent + tolerance) + min;

	float* iter = values;
	std::vector<int> indexes;
	auto pred = [minV,maxV](float i) { return i >= minV && i <= maxV; };
	while ((iter = std::find_if(iter, values + arraySize, pred)) != values + arraySize) {
		//TODO performance!!!	
		indexes.push_back(iter - values);
		iter++;
	}
	return indexes; //TODO should stay sorted
}

void InfluenceMapFloat::getIndexesWithByValue(float percent, float* intersection) const {
	const float diff = max - min;
	percent = fixValue(percent, 1);
	if (diff != 0.f) {
		for (int i = 0; i < arraySize; ++i) {
			auto val = percent - ((values[i] - min) / diff);
			intersection[i] += val * val;
		}
	}

}

void InfluenceMapFloat::add(int* indexes, float* vals, int k, float val) const {
	for (int j = 0; j < k; ++j) {
		values[indexes[j]] += val * vals[j];
	}
}

void InfluenceMapFloat::updateFromTemp() {
	if (tempComputedNeeded) {
		for (int i = 0; i < arraySize; ++i) {
			auto val = tempVals[i];
			if (val > 0.f) {
				update(i, val);
			}
		}
		std::fill_n(tempVals, arraySize, 0.f);
		tempComputedNeeded = false;
	}
}

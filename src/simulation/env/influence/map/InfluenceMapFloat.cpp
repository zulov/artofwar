#include "InfluenceMapFloat.h"
#include <algorithm>
#include <numeric>

#include "math/MathUtils.h"
#include "simulation/env/Environment.h"

InfluenceMapFloat::
InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug):
	InfluenceMap(resolution, size, valueThresholdDebug),
	coef(coef),
	level(level) {
	values = new float[arraySize];
}

InfluenceMapFloat::~InfluenceMapFloat() {
	delete[] values;
}

void InfluenceMapFloat::update(Physical* thing, float value) {
	auto& pos = thing->getPosition();

	const auto centerX = calculator.getIndex(pos.x_);
	const auto centerZ = calculator.getIndex(pos.z_);
	const auto minI = calculator.getValid(centerX - level);
	const auto maxI = calculator.getValid(centerX + level);

	const auto minJ = calculator.getValid(centerZ - level);
	const auto maxJ = calculator.getValid(centerZ + level);

	for (short i = minI; i <= maxI; ++i) {
		const auto a = (i - centerX) * (i - centerX);
		for (short j = minJ; j <= maxJ; ++j) {
			const auto b = (j - centerZ) * (j - centerZ);
			int index = calculator.getNotSafeIndex(i, j);
			values[index] += value / ((a + b) * coef + 1.f);
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
	auto index = calculator.indexFromPosition(pos);
	return getValueAt(index);
}

float InfluenceMapFloat::getValueAsPercent(const Urho3D::Vector2& pos) const {
	const float diff = max - min;
	if (diff != 0) {
		return (getValueAt(pos) - min) / diff;
	}
	return 0.5;
}

void InfluenceMapFloat::finishCalc() {
	const auto [minIdx, maxIdx] = std::minmax_element(values, values + arraySize);
	min = *minIdx;
	max = *maxIdx;
	avg = std::accumulate(values, values + arraySize, 0.0f) / arraySize;
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

	for (int i = 0; i < arraySize; ++i) {
		auto val = percent - ((values[i] - min) / diff);
		intersection[i] += val * val;
	}
}

void InfluenceMapFloat::add(int* indexes, float* vals, int k, float val) {
	for (int j = 0; j < k; ++j) {
		values[indexes[j]] += val * vals[j];
	}
}

#include "InfluenceMapFloat.h"
#include <algorithm>
#include <numeric>
#include "objects/Physical.h"
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

void InfluenceMapFloat::update(Physical* physical, float value) {
	auto iX = calculator.getIndex(physical->getPosition().x_);
	auto iZ = calculator.getIndex(physical->getPosition().z_);
	for (int i = iX - level; i < iX + level; ++i) {
		if (validIndex(i)) {
			for (int j = iZ - level; j < iZ + level; ++j) {
				if (validIndex(j)) {
					auto a = (i - iX) * (i - iX);
					auto b = (j - iZ) * (j - iZ);
					int index = calculator.getIndex(i, j);
					values[index] += (1 / ((a + b) * coef + 1)) * value;
				}
			}
		}
	}
}

void InfluenceMapFloat::reset() {
	std::fill_n(values, arraySize, 0.f);
}

bool InfluenceMapFloat::validIndex(int i) const {
	return i >= 0 && i < resolution;
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

void InfluenceMapFloat::calcStats() {
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

void InfluenceMapFloat::getIndexesWithByValue(float percent, const std::vector<float>& tolerances,
                                              unsigned char* intersection) const {
	const float diff = max - min;
	if (percent < 0) {
		percent = 0;
	} else if (percent < 1) {
		percent = 1;
	}

	auto minWeak = diff * (percent - tolerances.back()) + min;
	auto maxWeak = diff * (percent + tolerances.back()) + min;

	float* iter = values;
	std::vector<std::function<bool(float)>> predicates;

	for (int i = 0; i < tolerances.size() - 1; ++i) {
		auto minV = diff * (percent - tolerances[i]) + min;
		auto maxV = diff * (percent + tolerances[i]) + min;

		auto pred = [minV,maxV](float i) { return i >= minV && i <= maxV; };
		predicates.emplace_back(pred);
	}
	auto truePred = [](float i) { return true; };
	predicates.push_back(truePred);
	
	auto predWeek = [minWeak,maxWeak](float i) { return i >= minWeak && i <= maxWeak; };

	while ((iter = std::find_if(iter, values + arraySize, predWeek)) != values + arraySize) {
		for (int i = 0; i < predicates.size(); ++i) {
			if (predicates[i](*iter)) {
				intersection[iter - values] += predicates.size() - i;
				break;
			}
		}
		iter++;
	}
}

float InfluenceMapFloat::getFieldSize() {
	return calculator.getFieldSize();
}

#include "InfluenceMapFloat.h"
#include "objects/Physical.h"
#include <algorithm>
#include "simulation/env/Environment.h"
#include <numeric>

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

void InfluenceMapFloat::
getIndexesWithByValu1e(const float percent, float tolerance, unsigned char intersection[16384]) {
	const float diff = max - min;
	auto minV = diff * (percent - tolerance) + min;
	auto maxV = diff * (percent + tolerance) + min;

	float* iter = values;
	std::vector<int> indexes;
	auto pred = [minV,maxV](float i) { return i >= minV && i <= maxV; };
	while ((iter = std::find_if(iter, values + arraySize, pred)) != values + arraySize) {
		//TODO performance!!!
		++intersection[iter - values];
		iter++;
	}
}

void InfluenceMapFloat::getIndexesWithByValu2E(const float percent, const std::vector<float>& tolerances,
                                               unsigned char intersection[16384]) {
	const float diff = max - min;
	auto minV0 = diff * (percent - tolerances[0]) + min;
	auto maxV0 = diff * (percent + tolerances[0]) + min;

	auto minV1 = diff * (percent - tolerances[1]) + min;
	auto maxV1 = diff * (percent + tolerances[1]) + min;

	auto minV2 = diff * (percent - tolerances[2]) + min;
	auto maxV2 = diff * (percent + tolerances[2]) + min;

	float* iter = values;
	std::vector<int> indexes;
	auto pred0 = [minV0,maxV0](float i) { return i >= minV0 && i <= maxV0; };
	auto pred1 = [minV1,maxV1](float i) { return i >= minV1 && i <= maxV1; };
	auto pred2 = [minV2,maxV2](float i) { return i >= minV2 && i <= maxV2; };
	while ((iter = std::find_if(iter, values + arraySize, pred2)) != values + arraySize) {
		//TODO performance!!!
		if (pred0(*iter)) {
			intersection[iter - values] += 3;
		} else if (pred1(*iter)) {
			intersection[iter - values] += 2;
		} else {
			++intersection[iter - values];
		}
		iter++;
	}
}

float InfluenceMapFloat::getFieldSize() {
	return calculator.getFieldSize();
}

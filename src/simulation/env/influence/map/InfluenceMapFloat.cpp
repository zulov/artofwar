#include "InfluenceMapFloat.h"
#include "objects/Physical.h"
#include <algorithm>
#include "Game.h"
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

void InfluenceMapFloat::update(Physical* physical) {
	auto iX = calculator.getIndex(physical->getPosition().x_);
	auto iZ = calculator.getIndex(physical->getPosition().z_);
	for (int i = iX - level; i < iX + level; ++i) {
		if (validIndex(i)) {
			for (int j = iZ - level; j < iZ + level; ++j) {
				if (validIndex(j)) {
					auto a = (i - iX) * (i - iX);
					auto b = (j - iZ) * (j - iZ);
					int index = calculator.getIndex(i, j);
					values[index] += 1 / ((a + b) * coef + 1);
				}
			}
		}
	}
}

void InfluenceMapFloat::reset() {
	std::fill_n(values, arraySize, 0);
}

bool InfluenceMapFloat::validIndex(int i) const {
	return i >= 0 && i < resolution;
}

float InfluenceMapFloat::getValueAt(int index) const {
	return values[index];
}

Urho3D::Vector2 InfluenceMapFloat::getBestIndexToBuild(const short id) const {
	const auto [min, max] = std::minmax_element(values, values + arraySize);
	auto avg = std::accumulate(values, values + arraySize, 0) / (double)arraySize;

	float* iter = values;
	std::vector<int> indexes;
	while ((iter = std::find_if(iter, values + arraySize,
	                            [avg,max](float i) { return i > avg && i < *max; }))
		!= values + arraySize) {
		indexes.push_back(iter - values);
		iter++;
	}

	//print();
	return calculator.getCenter(indexes[rand() % indexes.size()]);
}

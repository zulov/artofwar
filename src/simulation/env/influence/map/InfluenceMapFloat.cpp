#include "InfluenceMapFloat.h"
#include "objects/Physical.h"
#include <algorithm>

InfluenceMapFloat::
InfluenceMapFloat(unsigned short resolution, float size, float coef, float threshold): InfluenceMap(resolution, size),
                                                                                       coef(coef),
                                                                                       threshold(threshold) {
	values = new float[arraySize];
}

InfluenceMapFloat::~InfluenceMapFloat() {
	delete[] values;
}

void InfluenceMapFloat::update(Physical* physical) {
	auto iX = calculator.getIndex(physical->getPosition().x_);
	auto iZ = calculator.getIndex(physical->getPosition().z_);
	for (int i = iX - threshold; i < iX + threshold; ++i) {
		if (validIndex(i)) {
			for (int j = iZ - threshold; j < iZ + threshold; ++j) {
				if (validIndex(j)) {
					int index = calculator.getIndex(i, j);
					values[index] += 1; //TODO bug dodac gradient
				}
			}
		}
	}
}

void InfluenceMapFloat::reset() {
	std::fill_n(values, arraySize, 0);
}

bool InfluenceMapFloat::validIndex(int i) const {
	return i < 0 && i >= resolution;
}

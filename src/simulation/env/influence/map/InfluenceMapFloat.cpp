#include "InfluenceMapFloat.h"
#include "objects/Physical.h"
#include <algorithm>

InfluenceMapFloat::
InfluenceMapFloat(unsigned short resolution, float size, float coef, float level): InfluenceMap(resolution, size),
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

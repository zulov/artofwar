#include "InfluenceMapFloat.h"
#include <algorithm>

InfluenceMapFloat::InfluenceMapFloat(unsigned short resolution): InfluenceMap(resolution) {
	values = new float[arraySize];
}

InfluenceMapFloat::~InfluenceMapFloat() {
	delete[] values;
}

void InfluenceMapFloat::update(Physical* physical) { }

void InfluenceMapFloat::reset() {
	std::fill_n(values, arraySize, 0);
}

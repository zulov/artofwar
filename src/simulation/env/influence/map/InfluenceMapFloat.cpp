#include "InfluenceMapFloat.h"
#include <algorithm>

InfluenceMapFloat::InfluenceMapFloat(unsigned short size): InfluenceMap(size) {
	values = new float[size];
}

InfluenceMapFloat::~InfluenceMapFloat() {
	delete[] values;
}

void InfluenceMapFloat::update(Physical* physical) { }

void InfluenceMapFloat::reset() {
	std::fill_n(values, size, 0);
}

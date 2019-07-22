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
	const int index = calculator.indexFromPosition(physical->getPosition());

}

void InfluenceMapFloat::reset() {
	std::fill_n(values, arraySize, 0);
}

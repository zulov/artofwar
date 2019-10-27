#include "InfluenceMapInt.h"
#include "objects/Physical.h"
#include <algorithm>

InfluenceMapInt::InfluenceMapInt(unsigned short resolution, float size, float valueThresholdDebug): InfluenceMap(
	resolution, size, valueThresholdDebug) {
	values = new unsigned char[arraySize];
}

InfluenceMapInt::~InfluenceMapInt() {
	delete[] values;
}

void InfluenceMapInt::update(Physical* physical, float value) {
	const int index = calculator.indexFromPosition(physical->getPosition());

	values[index] += round(value);
}

void InfluenceMapInt::reset() {
	std::fill_n(values, arraySize, 0);
}

char InfluenceMapInt::getValue(const Urho3D::Vector2& pos) {
	auto index = calculator.indexFromPosition(pos);
	return values[index];
}

float InfluenceMapInt::getValueAt(int index) const {
	return values[index];
}

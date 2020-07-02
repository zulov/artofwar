#include "InfluenceMapQTreeFloat.h"

#include "utils/DeleteUtils.h"

InfluenceMapQTreeFloat::InfluenceMapQTreeFloat(unsigned short resolution, float size, float coef, char level,
                                               float valueThresholdDebug, char childLevel)
	: InfluenceMapFloat(resolution, size, coef, level, valueThresholdDebug) {
	for (int i = 0; i < childLevel; ++i) {
		childValues.push_back(
			new InfluenceMapFloat(resolution >> (i + 1), size, coef, 0, valueThresholdDebug));
	}
}

InfluenceMapQTreeFloat::~InfluenceMapQTreeFloat() {
	clear_vector(childValues);
}

void InfluenceMapQTreeFloat::reset() {
	InfluenceMapFloat::reset();
	for (auto childValue : childValues) {
		childValue->reset();
	}
}

void InfluenceMapQTreeFloat::finishCalc() {
	InfluenceMapFloat::finishCalc();
	for (auto childValue : childValues) {
		childValue->finishCalc();
	}
}

void InfluenceMapQTreeFloat::update(Urho3D::Vector3& pos, float value) {
	InfluenceMapFloat::update(pos, value);
	for (auto childValue : childValues) {
		childValue->update(pos, value);
	}
}

#include "InfluenceMapHistory.h"


InfluenceMapHistory::InfluenceMapHistory(unsigned short resolution, float size, float coef, char level,
                                         float minimalThreshold, float vanishCoef, float valueThresholdDebug,
                                         float* sharedTemplateV, float* sharedTempVals):
	InfluenceMapFloat(resolution, size, coef, level, valueThresholdDebug, sharedTemplateV, sharedTempVals),
	minimalThreshold(minimalThreshold), vanishCoef(vanishCoef) {}

void InfluenceMapHistory::reset() {
	const auto end = values + arraySize;

	for (auto i = values; i < end; ++i) {
		*i *= vanishCoef;
	}
	minMaxInited = false;
}

void InfluenceMapHistory::resetToZero() const {
	const auto end = values + arraySize;

	for (auto i = values; i < end; ++i) {
		*i = *i >= minimalThreshold ? *i : 0.f;
	}
}

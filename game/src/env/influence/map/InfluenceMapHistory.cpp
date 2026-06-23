#include "InfluenceMapHistory.h"


InfluenceMapHistory::InfluenceMapHistory(unsigned short resolution, float size, float coef, char level,
                                         float minimalThreshold, float vanishCoef, float valueThresholdDebug,
                                         float* sharedTemplateV):
	InfluenceField(resolution, size, coef, level, valueThresholdDebug, sharedTemplateV),
	minimalThreshold(minimalThreshold), vanishCoef(vanishCoef) {}

void InfluenceMapHistory::reset() {
	const auto end = rawValues + arraySize;

	for (auto i = rawValues; i < end; ++i) {
		*i *= vanishCoef;
	}
	invalidateCaches();
}

void InfluenceMapHistory::resetToZero() {
	const auto end = rawValues + arraySize;

	for (auto i = rawValues; i < end; ++i) {
		*i = *i >= minimalThreshold ? *i : 0.f;
	}
	invalidateCaches();
}

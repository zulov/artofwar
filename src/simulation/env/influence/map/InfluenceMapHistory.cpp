#include "InfluenceMapHistory.h"


InfluenceMapHistory::InfluenceMapHistory(unsigned short resolution, float size, float coef, char level,
                                         float minimalThreshold, float vanishCoef, float valueThresholdDebug):
	InfluenceMapFloat(resolution, size, coef, level, valueThresholdDebug),
	minimalThreshold(minimalThreshold), vanishCoef(vanishCoef) {
}

void InfluenceMapHistory::reset() {
	for (int i = 0; i < arraySize; ++i) {
		if (values[i] < minimalThreshold) {
			values[i] = 0.f;
		} else {
			values[i] *= vanishCoef;
		}
	}
}

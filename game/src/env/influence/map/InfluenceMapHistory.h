#pragma once
#include "InfluenceField.h"

class InfluenceMapHistory : public InfluenceField {
public:
	InfluenceMapHistory(unsigned short resolution, float size, float coef, char level, float minimalThreshold,
	                    float vanishCoef, float valueThresholdDebug, float* sharedTemplateV);
	~InfluenceMapHistory() override = default;

	void reset() override;
	void resetToZero();
private:
	float minimalThreshold;
	float vanishCoef;
};

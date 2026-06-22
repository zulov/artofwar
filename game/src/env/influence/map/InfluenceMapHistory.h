#pragma once
#include "InfluenceMapFloat.h"

class InfluenceMapHistory : public InfluenceMapFloat {
public:
	InfluenceMapHistory(unsigned short resolution, float size, float coef, char level, float minimalThreshold,
	                    float vanishCoef, float valueThresholdDebug, float* sharedTemplateV);
	~InfluenceMapHistory() override = default;

	void reset() override;
	void resetToZero() const;
private:
	float minimalThreshold;
	float vanishCoef;
};

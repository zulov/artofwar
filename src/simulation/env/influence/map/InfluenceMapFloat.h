#pragma once
#include "InfluenceMap.h"

class InfluenceMapFloat : InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, float threshold);

	virtual ~InfluenceMapFloat();

	void update(Physical* physical) override;
	void reset() override;
private:
	bool validIndex(int i) const;
	float* values;
	float coef;
	float threshold;
};

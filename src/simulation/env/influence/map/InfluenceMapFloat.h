#pragma once
#include "InfluenceMap.h"

class InfluenceMapFloat : InfluenceMap {
public:
	InfluenceMapFloat(unsigned short size);

	virtual ~InfluenceMapFloat();

	void update(Physical* physical) override;
	void reset() override;
private:
	float* values;
};

#pragma once
#include "InfluenceMap.h"

#define VALUE_THRESHOLD_DEBUG 40

class InfluenceMapFloat : InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level);

	virtual ~InfluenceMapFloat();

	void update(Physical* physical) override;
	void reset() override;
	void drawCell(int index, short batch);

	void draw(short batch, short maxParts) override;
private:
	bool validIndex(int i) const;
	float* values;
	float coef;
	char level;
};

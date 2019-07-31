#pragma once
#include "InfluenceMap.h"

class InfluenceMapFloat : InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level);

	virtual ~InfluenceMapFloat();

	void update(Physical* physical) override;
	void reset() override;
	void drawCell(int index);

	void draw() override;
private:
	bool validIndex(int i) const;
	float* values;
	float coef;
	char level;
};

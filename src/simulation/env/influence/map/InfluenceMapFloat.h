#pragma once
#include "InfluenceMap.h"


class InfluenceMapFloat : public InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level);

	virtual ~InfluenceMapFloat();

	void update(Physical* physical) override;
	void reset() override;
	float getValueAt(int index) override;
private:
	bool validIndex(int i) const;
	float* values;
	float coef;
	char level;
};

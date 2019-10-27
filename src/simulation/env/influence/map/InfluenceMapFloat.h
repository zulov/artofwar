#pragma once
#include "InfluenceMap.h"


class InfluenceMapFloat : public InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug);
	virtual ~InfluenceMapFloat();

	void update(Physical* physical, float value=1) override;
	void reset() override;
	float getValueAt(int index) const override;
	Urho3D::Vector2 getBestIndexToBuild(const short id) const;
private:
	bool validIndex(int i) const;
	float* values;
	float coef;
	char level;
};

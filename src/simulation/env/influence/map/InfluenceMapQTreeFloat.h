#pragma once
#include "InfluenceMapFloat.h"

class InfluenceMapQTreeFloat : public InfluenceMapFloat {
public:

	InfluenceMapQTreeFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug,
	                       char childLevel);
	~InfluenceMapQTreeFloat();
	void reset() override;
	void finishCalc() override;
	void update(Urho3D::Vector3& pos, float value = 1) override;
private:
	std::vector<InfluenceMapFloat*> childValues;
};

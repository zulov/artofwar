#pragma once
#include <vector>

#include "InfluenceMap.h"
#include "InfluenceMapFloat.h"

class InfluenceMapCombine : public InfluenceMap {
public:
	InfluenceMapCombine(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug);
	~InfluenceMapCombine();

	void update(Urho3D::Vector3& pos, float value) override;
	void update(Physical* thing, float value);
	void reset() override;
	float getValueAt(int index) const override;
	void finishCalc() override;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
private:
	std::vector<InfluenceMapFloat*> values;
	float coef;
	unsigned char level;

	int* indexes;
	float* vals;

	short prevCenterX = -1;
	short prevCenterZ = -1;
	int k = 0;
	int q = 0;
	int w = 0;
};

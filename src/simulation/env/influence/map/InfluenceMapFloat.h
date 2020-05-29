#pragma once
#include "InfluenceMap.h"
#include <vector>


class InfluenceMapFloat : public InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug);
	virtual ~InfluenceMapFloat();

	void update(Physical* physical, float value=1) override;
	void reset() override;
	float getValueAt(int index) const override;
	float getValueAt(const Urho3D::Vector2& pos) const;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	void calcStats() override;
	std::vector<int> getIndexesWithByValue(float percent, float tolerance) const;
	float getFieldSize();
	void getIndexesWithByValu1e(const float percent, float tolerance, unsigned char intersection[16384]);
	void getIndexesWithByValu2E(const float percent, const std::vector<float>& tolerances, unsigned char intersection[16384]);
private:
	bool validIndex(int i) const;
	float* values;
	float coef;
	char level;
};

#pragma once
#include "InfluenceMap.h"
#include <vector>


class InfluenceMapFloat : public InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug);
	virtual ~InfluenceMapFloat();

	void update(Urho3D::Vector3& pos, float value=1) override;
	void reset() override;
	float getValueAt(int index) const override;
	float getValueAt(const Urho3D::Vector2& pos) const;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	void calcStats() override;
	std::vector<int> getIndexesWithByValue(float percent, float tolerance) const;
	float getFieldSize();
	void getIndexesWithByValue(float percent, float *intersection) const;
private:
	bool validIndex(int i) const;
	float* values;
	float coef;
	unsigned char level;
};

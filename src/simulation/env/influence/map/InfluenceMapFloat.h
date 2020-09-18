#pragma once
#include "InfluenceMap.h"
#include <vector>


class InfluenceMapFloat : public InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug);
	virtual ~InfluenceMapFloat();

	void update(Physical* thing, float value = 1) override;
	void reset() override;
	float getValueAt(int index) const override;
	float getValueAt(const Urho3D::Vector2& pos) const;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	float getValueAsPercent(int index) const override;
	void finishCalc() override;
	int getMaxElement() override;
	int getMaxElement(std::array<int, 4> indexes) override;
	std::vector<int> getIndexesWithByValue(float percent, float tolerance) const;
	void getIndexesWithByValue(float percent, float* intersection) const;
	void add(int* indexes, float* vals, int k, float val);
protected:
	float* values;
	float coef;
private:

	unsigned char level;
};

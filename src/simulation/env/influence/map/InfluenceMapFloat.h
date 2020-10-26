#pragma once
#include "InfluenceMap.h"
#include <vector>


class InfluenceMapFloat : public InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug);
	virtual ~InfluenceMapFloat();

	void update(Physical* thing, float value = 1) override;
	void update(int index, float value);
	void update1(float* data);
	void reset() override;
	float getValueAt(int index) const override;
	float getValueAt(const Urho3D::Vector2& pos) const;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	float getValueAsPercent(int index) const override;
	void finishCalc() override;
	int getMaxElement() override;
	int getMaxElement(const std::array<int, 4>& indexes) override;
	std::vector<int> getIndexesWithByValue(float percent, float tolerance) const;
	void getIndexesWithByValue(float percent, float* intersection) const;
	void add(int* indexes, float* vals, int k, float val);
	int getIndex(const Urho3D::Vector3& pos) const;

	void addTemp(int idx, float val) const { tempVals[idx] += val; }
	void updateFromTemp();

protected:
	float* values;
	float* tempVals;
	float coef;
private:

	unsigned char level;
};

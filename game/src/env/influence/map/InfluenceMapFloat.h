#pragma once
#include <vector>

#include "InfluenceMap.h"



class InfluenceMapFloat : public InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug);
	virtual ~InfluenceMapFloat();

	void update(Physical* thing, float value = 1.f) override;
	void updateInt(Physical* thing, int value = 1) override;

	void tempUpdate(const Urho3D::Vector3& pos, float value = 1.f);
	void tempUpdate(int index, float value = 1.f);
	void update(int index) const;
	void reset() override;
	float getValueAt(int index) const override;
	float getValueAt(const Urho3D::Vector2& pos) const;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	float getValueAsPercent(int index) const override;
	void computeMinMax() override;
	void ensureReady() override;

	std::vector<int> getIndexesWithByValue(float percent, float tolerance);
	bool cumulateErros(float percent, float* intersection) const;

	void updateFromTemp();

protected:
	float* values;
	float* tempVals;
	float coef;
	bool valuesCalculateNeeded = false;
	std::vector<int> changedIndexes;
private:
	void update(float value, unsigned short centerX, unsigned short centerZ) const;

	unsigned char level;
	unsigned char levelRes;
	float* templateV;
};

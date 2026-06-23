#pragma once
#include <vector>

#include "InfluenceMap.h"



class InfluenceMapFloat : public InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug,
	                  float* sharedTemplateV);
	~InfluenceMapFloat() override;

	void update(const Urho3D::Vector2& pos, float value = 1.f);
	void update(int index, float value = 1.f);

	virtual void reset();

	void ensureReady() override;

	std::vector<int> getIndexesWithByValue(float percent, float tolerance);
	bool cumulateErrors(float percent, float* intersection);

	void updateFromTemp();

	static float* createTemplateV(float coef, char level);

protected:
	float getValueAt(unsigned index) const override;
	float* values;
	float* tempVals;
	float coef;
	bool valuesCalculateNeeded = false;
	std::vector<int> changedIndexes;
private:
	void applyKernel(int index) const;
	void computeMinMax();
	unsigned char level;
	unsigned char levelRes;
	float* templateV;
};

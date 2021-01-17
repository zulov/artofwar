#pragma once
#include "InfluenceMap.h"
#include <vector>

#include "objects/unit/Unit.h"


class InfluenceMapFloat : public InfluenceMap {
public:
	InfluenceMapFloat(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug);
	virtual ~InfluenceMapFloat();

	void update(Physical* thing, float value = 1.f) override;
	void updateInt(Physical* thing, int value = 1) override;
	void tempUpdate(Physical* thing, float value = 1.f);
	void tempUpdate(int index, float value = 1.f);
	void update(int index, float value) const;
	void reset() override;
	float getValueAt(int index) const override;
	float getValueAt(const Urho3D::Vector2& pos) const;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	float getValueAsPercent(int index) const override;
	void computeMinMax() override;

	int getMaxElement() override;
	int getMaxElement(const std::array<int, 4>& indexes) override;
	std::vector<int> getIndexesWithByValue(float percent, float tolerance) const;
	void getIndexesWithByValue(float percent, float* intersection) const;
	void add(int* indexes, float* vals, int k, float val) const;

	void updateFromTemp() const;

protected:
	float* values;
	float coef;
private:
	void update(float value, unsigned short centerX, unsigned short centerZ) const;

	float* tempVals;
	unsigned char level;
	bool tempComputedNeeded = false;
};

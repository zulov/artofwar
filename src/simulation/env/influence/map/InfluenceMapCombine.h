#pragma once
#include <span>
#include <vector>

#include "InfluenceMap.h"
#include "InfluenceMapFloat.h"

class InfluenceMapCombine : public InfluenceMap {
public:
	InfluenceMapCombine(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug,
		char numberOfMaps);
	~InfluenceMapCombine();

	void update(Physical* thing, float value) override;
	void update(Physical* thing, std::span<float> values1);
	void reset() override;
	float getValueAt(int index) const override;
	void finishCalc() override;
	float getValueAsPercent(const Urho3D::Vector2& pos) const override;
	InfluenceMapFloat* get(char index);
	void print(Urho3D::String name) override;
private:
	std::vector<InfluenceMapFloat*> values;
	float coef;
	unsigned char level;
	unsigned char numberOfMaps;

	int* indexes;
	float* vals;

	short prevCenterX = -1;
	short prevCenterZ = -1;
	int k = 0;
};

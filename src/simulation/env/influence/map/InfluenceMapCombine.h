#pragma once
#include <span>
#include <vector>
#include "InfluenceMapFloat.h"

class InfluenceMapCombine {
public:
	InfluenceMapCombine(unsigned short resolution, float size, float coef, char level, float valueThresholdDebug,
	                    char numberOfMaps);
	~InfluenceMapCombine();

	void update(Physical* thing, std::span<float> values1);
	void reset();
	void finishCalc();
	InfluenceMapFloat* get(char index);
	void print(Urho3D::String name);
private:
	std::vector<InfluenceMapFloat*> values;
	GridCalculator* calculator;
	float coef;
	unsigned char level;
	unsigned char numberOfMaps;

	int* indexes;
	float* vals;

	short prevCenterX = -1;
	short prevCenterZ = -1;
	int k = 0;
};

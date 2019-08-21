#pragma once
#include "simulation/env/GridCalculator.h"


class Physical;

class InfluenceMap {

public:
	InfluenceMap(unsigned short resolution, float size, float valueThresholdDebug);
	virtual ~InfluenceMap();

	virtual void update(Physical* physical) =0;
	virtual void reset() =0;
	virtual float getValueAt(int index) =0;
	void draw(short batch, short maxParts);
	void drawCell(int index, short batch);
protected:
	unsigned short resolution;
	float size;
	float fieldSize;
	unsigned int arraySize;
	const float valueThresholdDebug;
	GridCalculator calculator;
};

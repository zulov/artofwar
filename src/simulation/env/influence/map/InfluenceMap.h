#pragma once
#include "simulation/env/GridCalculator.h"
class Physical;

class InfluenceMap {

public:
	InfluenceMap(unsigned short resolution, float size);
	virtual ~InfluenceMap();

	virtual void update(Physical* physical) =0;
	virtual void reset() =0;
	virtual void draw(short batch, short maxParts) =0;
protected:
	unsigned short resolution;
	float size;
	float fieldSize;
	unsigned int arraySize;
	GridCalculator calculator;
};

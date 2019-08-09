#pragma once
#include "simulation/env/GridCalculator.h"


#define VALUE_THRESHOLD_DEBUG 40

class Physical;

class InfluenceMap {

public:
	InfluenceMap(unsigned short resolution, float size);
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
	GridCalculator calculator;
};

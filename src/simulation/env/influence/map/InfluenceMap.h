#pragma once
#include "simulation/env/GridCalculator.h"


class Physical;

class InfluenceMap {

public:
	InfluenceMap(unsigned short resolution, float size, float valueThresholdDebug);
	virtual ~InfluenceMap();

	virtual void update(Physical* physical, float value = 1) =0;
	virtual void reset() =0;
	virtual float getValueAt(int index) const =0;

	virtual void calcStats() =0;
	void draw(short batch, short maxParts);
	void drawCell(int index, short batch) const;
		Urho3D::Vector2 getCenter(int value);
	virtual float getValueAsPercent(const Urho3D::Vector2& pos) const =0;
protected:
	void print() const;
	unsigned short resolution;
	float size;
	float fieldSize;
	unsigned int arraySize;
	const float valueThresholdDebug;

	float avg;
	float min;
	float max;
	GridCalculator calculator;
};

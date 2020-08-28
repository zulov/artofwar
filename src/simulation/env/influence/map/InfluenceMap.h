#pragma once
#include "simulation/env/GridCalculator.h"

class Physical;

class InfluenceMap {
public:
	InfluenceMap(unsigned short resolution, float size, float valueThresholdDebug);
	virtual ~InfluenceMap() = default;

	virtual void update(Physical* thing, float value = 1) =0;
	virtual void reset() =0;
	virtual float getValueAt(int index) const =0;

	virtual void finishCalc() =0;
	void draw(short batch, short maxParts) const;
	void drawCell(int index, short batch) const;
	Urho3D::Vector2 getCenter(int index) const;
	float getFieldSize() const;
	virtual float getValueAsPercent(const Urho3D::Vector2& pos) const =0;
	virtual float getValueAsPercent(const int index) const =0;
	virtual void print(Urho3D::String name);
protected:

	float size;
	float fieldSize;
	unsigned int arraySize;
	const float valueThresholdDebug;

	float avg;
	float min;
	float max;
	int counter = 0;
	unsigned short resolution;
	GridCalculator calculator;
};
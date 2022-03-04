#pragma once
#include "InfluenceMapI.h"
#include "simulation/env/GridCalculator.h"

struct GridCalculator;
class Physical;

class InfluenceMap : public InfluenceMapI {
public:
	InfluenceMap(unsigned short resolution, float size, float valueThresholdDebug);
	virtual ~InfluenceMap() = default;

	virtual float getValueAt(int index) const =0;
	virtual void computeMinMax() = 0;
	virtual void ensureReady() = 0;
	void draw(short batch, short maxParts);
	Urho3D::Vector3 getVertex(const Urho3D::Vector2 center, Urho3D::Vector2 vertex) const;
	void drawCell(int index, short batch) const;
	Urho3D::Vector2 getCenter(int index) const;
	float getFieldSize() const;
	virtual float getValueAsPercent(const Urho3D::Vector2& pos) const =0;
	virtual float getValueAsPercent(const int index) const =0;
	void print(Urho3D::String name) override;
	unsigned short getResolution() const override { return calculator->getResolution(); }
	Urho3D::Vector2 getCenter(int index) override { return calculator->getCenter(index); }
protected:
	GridCalculator* calculator;
	unsigned int arraySize;
	const float valueThresholdDebug;

	float min;
	float max;

	unsigned short counter = 0;

	bool minMaxInited = false;
};

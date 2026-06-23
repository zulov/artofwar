#pragma once
#include <Urho3D/Math/Vector3.h>

#include "env/GridCalculator.h"

struct GridCalculator;
class Physical;

class InfluenceMap {
public:
	InfluenceMap(unsigned short resolution, float size, float valueThresholdDebug);
	virtual ~InfluenceMap() = default;

	virtual float getValueAt(unsigned index) const =0;
	virtual void ensureReady() = 0;
	void draw(short batch, short maxParts);

	void print(Urho3D::String name);
	unsigned short getResolution() const { return calculator->getResolution(); }
protected:
	GridCalculator* calculator;
	unsigned int arraySize;
	const float valueThresholdDebug;

	float min;
	float max;
	int minIdx;
	int maxIdx;

	unsigned short counter = 0;

	bool minMaxInited = false;

private:
	float getValueAsPercent(unsigned index) const;
	void drawCell(int index, short batch) const;
	Urho3D::Vector3 getVertex(const Urho3D::Vector2& center, Urho3D::Vector2 vertex) const;
};

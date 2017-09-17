#pragma once
#include <Urho3D/Math/Vector2.h>
#include "objects/Physical.h"

class Gradient
{
public:
	Gradient(short _resolution, double _size);
	~Gradient();
	Urho3D::Vector2&  getValueAt(double x, double z);
	int getIntegerPos(double value);
	double getDoublePos(double value);
	
	void add(Physical * entity);
	double calculateCoef(double distance, double minDist);
	void remove(Physical * entity);
private:
	Urho3D::Vector2** values;
	short resolution;
	short halfResolution;
	double size;
	double fieldSize;
};


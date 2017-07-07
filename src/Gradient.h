#pragma once
#include <Urho3D/Math/Vector3.h>
#include "Entity.h"

class Entity;

class Gradient
{
public:
	Gradient(short _resolution, double _size);
	~Gradient();
	Urho3D::Vector3* getValueAt(double x, double z);
	int getIntegerPos(double value);
	double getDoublePos(double value);
	
	void add(Entity * entity);
	double calculateCoef(double distance, double minDist);
	void remove(Entity * entity);
private:
	Urho3D::Vector3*** values;
	short resolution;
	short halfResolution;
	double size;
	double fieldSize;
};


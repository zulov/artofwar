#pragma once
#include <Urho3D/Math/Vector3.h>

class Entity;

class Gradient
{
public:
	Gradient(int _resolution, double _size);
	~Gradient();
	Urho3D::Vector3* getValueAt(int x, int z);
	void add(Entity * entity);
	void remove(Entity * entity);
private:
	Urho3D::Vector3*** values;
	int resolution;
	int halfResolution;
	double size;
	double fieldSize;
};


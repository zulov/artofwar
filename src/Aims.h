#pragma once
#include <Urho3D/Math/Vector3.h>
#include <vector>
#include "Entity.h"

class Entity;

class Aims
{
public:
	Aims();
	~Aims();
	Urho3D::Vector3* getAim();
	bool check(Urho3D::Vector3* pedestrian);
	void add(Entity* entity);
private:
	std::vector<Urho3D::Vector3*>* aims;
	double radius;
	int index;
};

#pragma once
#include "Aim.h"
#include <Urho3D/Math/Vector3.h>
#include <vector>


class Entity;

class Aims
{
public:
	Aims();
	~Aims();
	Urho3D::Vector3* getDirection(Unit* unit);
	void clearAims();
	bool ifReach(Unit* unit);
	void add(Aim* aim);
private:
	std::vector<Aim*> aims;
	int current;
};

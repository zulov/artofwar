#pragma once
#include <Urho3D/Math/Vector3.h>
#include <vector>
#include "Aim.h"

class Entity;

class Aims
{
public:
	Aims();
	~Aims();
	Urho3D::Vector3* getDirection(Unit* unit, short index);
	void clearAims();
	bool ifReach(Unit* unit, int index);
	void add(Aim* aim);
	bool check(int aimIndex);
	int getReferences();
	void reduce();
	void up();
private:
	std::vector<Aim*> aims;
	short int references;
};

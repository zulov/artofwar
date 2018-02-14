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
	void clearAimsIfExpired();
	bool ifReach(Unit* unit);
	bool hasAim();
	void add(Aim* aim);
	void clear();
private:
	std::vector<Aim*> aims;
	int current;
};

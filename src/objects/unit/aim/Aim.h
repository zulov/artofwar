#pragma once
#include <Urho3D/Math/Vector3.h>


class Unit;


class Aim
{
public:
	Aim();
	virtual ~Aim();
	virtual Urho3D::Vector3* getDirection(Unit* unit) = 0;
	virtual bool ifReach(Unit* unit) = 0;
	virtual bool expired() = 0;
};
#pragma once
#include <Urho3D/Math/Vector3.h>
#include <vector>


class Unit;

class Aim
{
public:

	virtual ~Aim() = default;
	virtual Urho3D::Vector2 getDirection(Unit* unit) = 0;
	virtual bool ifReach(Unit* unit) = 0;
	virtual bool expired() = 0;
	virtual std::vector<Urho3D::Vector3> getDebugLines(Urho3D::Vector3* position) { return {}; }
};

#pragma once
#include "Aim.h"
#include <Urho3D/Math/Vector3.h>

class Static;

class FollowStaticAim :
	public Aim
{
public:
	explicit FollowStaticAim(Static* _staticObj);
	~FollowStaticAim();
	Urho3D::Vector2 getDirection(Unit* unit) override;
	bool ifReach(Unit* unit) override;
	bool expired() override;
private:

	Static* staticObj;
	float radiusSq;
};

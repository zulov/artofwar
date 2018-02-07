#pragma once
#include "Aim.h"
#include "objects/Physical.h"

class FollowAim : public Aim
{
public:
	FollowAim(Physical* _physical);
	~FollowAim();
	Urho3D::Vector3* getDirection(Unit* unit) override;
	bool ifReach(Unit* unit) override;
	bool expired() override;
private:
	Physical* physical;
	double radiusSq;
};

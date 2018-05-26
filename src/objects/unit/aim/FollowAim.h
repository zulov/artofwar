#pragma once
#include "Aim.h"

class Physical;

class FollowAim : public Aim
{
public:
	explicit FollowAim(const Physical* _physical);
	~FollowAim();
	Urho3D::Vector2 getDirection(Unit* unit) override;
	bool ifReach(Unit* unit) override;
	bool expired() override;
private:
	const Physical* physical;
	float radiusSq;
};

#pragma once
#include "Aim.h"
#include <vector>

class TargetAim;
class Physical;

class FollowAim : public Aim {
public:
	explicit FollowAim(Physical* physical, TargetAim* subTarget);
	~FollowAim();

	std::vector<Urho3D::Vector3> getDebugLines(Unit* follower) const override;
	Urho3D::Vector2 getDirection(Unit* follower) override;
	bool ifReach(Unit* follower) override;
	bool expired() override;
private:
	Physical* physical;
	TargetAim* subTarget;
	float radiusSq;
};

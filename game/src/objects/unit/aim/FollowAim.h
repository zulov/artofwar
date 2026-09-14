#pragma once
#include <vector>
#include "Aim.h"

class TargetAim;
class Physical;

class FollowAim : public Aim {
public:
	explicit FollowAim(Physical* physical, TargetAim* subTarget);
	~FollowAim() override;

	std::vector<Urho3D::Vector3> getDebugLines(Unit* follower) const override;
	Urho3D::Vector2 getDirection(Unit* follower) override;
	bool ifReach(Unit* follower) override;
	bool expired() override;
	AimSaveData saveState() const override;

private:
	Physical* physical;
	TargetAim* subTarget;
	float radiusSq;
};

#pragma once
#include "Aim.h"
#include <vector>

class TargetAim;
class Physical;

class FollowAim : public Aim
{
public:
	explicit FollowAim(const Physical* physical,TargetAim* subTarget);
	~FollowAim();

	std::vector<Urho3D::Vector3> getDebugLines(Urho3D::Vector3* position) const override;
	Urho3D::Vector2 getDirection(Unit* unit) override;
	bool ifReach(Unit* unit) override;
	bool expired() override;
private:
	const Physical* physical;
	TargetAim* subTarget;
	float radiusSq;
};

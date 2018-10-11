#pragma once
#include "Aim.h"
#include <Urho3D/Scene/Node.h>
#include <vector>

class TargetAim : public Aim
{
public:
	explicit TargetAim(std::vector<int>& _path);
	~TargetAim();

	std::vector<Urho3D::Vector3> getDebugLines(Unit* unit) const override;
	Urho3D::Vector2 getDirection(Unit* unit) override;
	bool ifReach(Unit* unit) override;
	bool expired() override;
private:
	float radiusSq;

	std::vector<int> path;
	short current;
	Urho3D::Vector2 currentTarget;
};

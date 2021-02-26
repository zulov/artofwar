#pragma once
#include "Aim.h"
#include <vector>

class TargetAim : public Aim {
public:
	explicit TargetAim(std::vector<int> _path);
	~TargetAim() = default;

	std::vector<Urho3D::Vector3> getDebugLines(Unit* unit) const override;
	Urho3D::Vector2 getDirection(Unit* unit) override;
	bool ifReach(Unit* unit) override;
	bool expired() override;
private:
	std::vector<int> path;
	Urho3D::Vector2 currentTarget;
	short current;
};

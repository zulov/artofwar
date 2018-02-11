#pragma once
#include "Aim.h"
#include <Urho3D/Scene/Node.h>
#include <vector>

class TargetAim : public Aim
{
public:
	TargetAim(std::vector<int> &_path);
	~TargetAim();
	Urho3D::Vector3* getDirection(Unit* unit) override;
	bool ifReach(Unit* unit) override;
	bool expired() override;
private:
	double radiusSq;

	std::vector<int> path;
	short current;
};

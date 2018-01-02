#pragma once
#include "Aim.h"
#include <Urho3D/Scene/Node.h>

class TargetAim : public Aim
{
public:
	TargetAim(Urho3D::Vector3* _position);
	~TargetAim();
	Urho3D::Vector3* getDirection(Unit* unit) override;
	bool ifReach(Unit* unit) override;
	bool expired() override;
private:
	Urho3D::Vector3 position;
	double radius;
	Urho3D::Node* node;
};

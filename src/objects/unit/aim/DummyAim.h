#pragma once
#include "Aim.h"

class DummyAim : public Aim
{
public:
	DummyAim();
	~DummyAim();
	Urho3D::Vector3* getDirection(Unit* unit) override;
	bool ifReach(Unit* unit) override;
	bool expired() override;
};


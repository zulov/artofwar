#pragma once
#include "Aim.h"

class DummyAim : public Aim//TODO zrobic z tego singleton
{
public:
	DummyAim();
	~DummyAim() override;
	Urho3D::Vector3* getDirection(Unit* unit) override;
	bool ifReach(Unit* unit) override;
	bool expired() override;
};


#pragma once
#include "Aim.h"

class ChargeAim : public Aim
{
public:
	explicit ChargeAim(Urho3D::Vector2* _direction);
	~ChargeAim() = default;

	std::vector<Urho3D::Vector3> getDebugLines(Unit* unit) const override;
	Urho3D::Vector2 getDirection(Unit* unit) override;
	bool ifReach(Unit* unit) override;
	bool expired() override;
private:
	Urho3D::Vector2 direction;
	float chargeEnergy = 20 * 10;
};

#pragma once
#include "UnitOrder.h"
#include "objects/Physical.h"

class FutureOrder
{
public:
	FutureOrder(const Urho3D::Vector2& vector, const Physical* physical, UnitOrder action);
	FutureOrder(const Urho3D::Vector2& vector, UnitOrder action);
	FutureOrder(const Physical* physical, UnitOrder action);
	bool expired() const;
	void execute();
	ActionParameter getTargetAim(int startInx, Urho3D::Vector2& to);
	ActionParameter getFollowAim(int startInx, Urho3D::Vector2& toSoFar, const Physical* toFollow);
	ActionParameter getChargeAim(Urho3D::Vector2* charge);
private:
	virtual void addTargetAim(Urho3D::Vector2* to) =0;
	virtual void addFollowAim(const Physical* toFollow) =0;
	virtual void addChargeAim(Urho3D::Vector2* charge) =0;
	virtual void addDeadAim() =0;

	virtual void addAttackAim(const Physical* physical) =0;
	virtual void addDefendAim() =0;

	Urho3D::Vector2 vector;
	const Physical* physical;
	UnitOrder action;
};

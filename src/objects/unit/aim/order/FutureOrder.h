#pragma once
#include "UnitOrder.h"
#include "objects/Physical.h"

class FutureOrder
{
public:
	FutureOrder(UnitOrder action, const Urho3D::Vector2& vector, const Physical* toUse);
	FutureOrder(UnitOrder action, const Urho3D::Vector2& vector);
	FutureOrder(UnitOrder action, const Physical* toUse);

	virtual ~FutureOrder();

	bool expired() const;
	virtual void add(bool append) =0;
	ActionParameter getTargetAim(int startInx, Urho3D::Vector2& to);
	ActionParameter getFollowAim(int startInx, Urho3D::Vector2& toSoFar, const Physical* toFollow);
	ActionParameter getChargeAim(Urho3D::Vector2* charge);
	void execute();
protected:
	Urho3D::Vector2 vector;
	const Physical* toUse;
	UnitOrder action;
private:
	virtual void addTargetAim() =0;
	virtual void addFollowAim() =0;
	virtual void addChargeAim() =0;
	virtual void addAttackAim() =0;
	virtual void addDefendAim() =0;
	virtual void addDeadAim() =0;
};

#pragma once
#include "UnitOrder.h"
#include "objects/Physical.h"

class FutureOrder
{
public:
	FutureOrder(UnitOrder action, bool append, const Urho3D::Vector2& vector, Physical* toUse);
	FutureOrder(UnitOrder action, bool append, const Urho3D::Vector2& vector);
	FutureOrder(UnitOrder action, bool append, Physical* toUse);

	virtual ~FutureOrder();

	bool expired() const;
	virtual bool add() =0;
	ActionParameter getTargetAim(int startInx, Urho3D::Vector2& to);
	ActionParameter getFollowAim(int startInx, Urho3D::Vector2& toSoFar, Physical* toFollow);
	ActionParameter getChargeAim(Urho3D::Vector2& charge);

	void execute();
	bool getAppend() const { return append; }
	UnitOrder getAction() const;
protected:
	Physical* toUse;
	Urho3D::Vector2 vector;
	UnitOrder action;
	const bool append;
private:
	virtual void addCollectAim() =0;
	virtual void addTargetAim() =0;
	virtual void addFollowAim() =0;
	virtual void addChargeAim() =0;
	virtual void addAttackAim() =0;
	virtual void addDefendAim() =0;
	virtual void addDeadAim() =0;
	virtual void addStopAim() =0;
};

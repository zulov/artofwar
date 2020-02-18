#pragma once
#include "FutureOrder.h"

class UnitOrder : public FutureOrder {
	void execute() override;
	ActionParameter getTargetAim(int startInx, Urho3D::Vector2& to);
	ActionParameter getFollowAim(int startInx, Urho3D::Vector2& toSoFar, Physical* toFollow);
	ActionParameter getChargeAim(Urho3D::Vector2& charge);
private:
	Physical* toUse;
	Urho3D::Vector2 vector;

	virtual void addCollectAim() =0;
	virtual void addTargetAim() =0;
	virtual void addFollowAim() =0;
	virtual void addChargeAim() =0;
	virtual void addAttackAim() =0;
	virtual void addDefendAim() =0;
	virtual void addDeadAim() =0;
	virtual void addStopAim() =0;
};

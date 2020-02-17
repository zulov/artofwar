#pragma once
#include "objects/Physical.h"

class FutureOrder {
public:
	FutureOrder(short actionType, short id, bool append);

	virtual ~FutureOrder();

	virtual bool expired() =0;
	virtual bool clean() =0;
	virtual bool add() =0;
	ActionParameter getTargetAim(int startInx, Urho3D::Vector2& to);
	ActionParameter getFollowAim(int startInx, Urho3D::Vector2& toSoFar, Physical* toFollow);
	ActionParameter getChargeAim(Urho3D::Vector2& charge);

	void execute();
	bool getAppend() const { return append; }
	short getAction() const { return action; }
protected:
	const short action;
	const short id;

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

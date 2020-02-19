#pragma once
#include "FutureOrder.h"
#include "enums/UnitActionType.h"
#include "enums/UnitActionType.h"

enum class UnitAction : char;
enum class UnitActionType : char;

class UnitOrder : public FutureOrder {
public:
	UnitOrder(UnitActionType actionType, UnitAction id, bool append, Physical* toUse, const Urho3D::Vector2& vector);
	void execute() override;
protected:
	Physical* toUse;
	Urho3D::Vector2 vector;

	ActionParameter getTargetAim(int startInx, Urho3D::Vector2& to);
	ActionParameter getFollowAim(int startInx, Urho3D::Vector2& toSoFar, Physical* toFollow);
	ActionParameter getChargeAim(Urho3D::Vector2& charge);
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

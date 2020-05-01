#pragma once
#include "objects/unit/ActionParameter.h"

class Physical;

namespace Urho3D
{
	class Vector2;
}

enum class UnitAction : char;
enum class UnitActionType : char;

class UnitOrder {
public:
	UnitOrder(UnitActionType actionType, short id, bool append, Physical* toUse);
	UnitOrder(UnitActionType actionType, short id, bool append, Urho3D::Vector2& vector);
	virtual ~UnitOrder();
	virtual void execute();

	virtual bool expired() =0;
	virtual bool add() =0;
	virtual void clean() =0;

	bool getAppend() const { return append; }
	short getId() const { return id; }
protected:
	Physical* toUse = nullptr;
	Urho3D::Vector2* vector = nullptr;

	const UnitActionType actionType;
	const bool append;
	const short id;

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

#pragma once
#include "FutureOrder.h"
#include "UnitOrder.h"

class Physical;

namespace Urho3D {
	class Vector2;
}

enum class UnitAction : char;
class Formation;

class FormationOrder : public UnitOrder {
public:
	FormationOrder(Formation* formation, UnitActionType actionType, short action, const Urho3D::Vector2& vector,
	               Physical* toUse, bool append = false);
	~FormationOrder();
	bool add() override;
	bool expired() override;
	void clean() override;
private:
	Formation* formation;

	void addCollectAim() override;
	void addTargetAim() override;
	void addFollowAim() override;
	void addChargeAim() override;
	void addAttackAim() override;
	void addDefendAim() override;
	void addDeadAim() override;
	void addStopAim() override;

	void simpleAction() const;
	void followAndAct(float distThreshold);
};

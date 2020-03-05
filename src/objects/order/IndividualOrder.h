#pragma once
#include "UnitOrder.h"

enum class UnitAction : char;

class IndividualOrder : public UnitOrder {
public:
	IndividualOrder(Unit* unit, UnitAction action, Urho3D::Vector2& vector, Physical* toUse, bool append = false);
	IndividualOrder(Unit* unit, UnitAction action, Physical* toUse, bool append = false);
	~IndividualOrder();

	bool expired() override;
	bool add() override;
	void clean() override;
private:
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

	Unit* unit;
};

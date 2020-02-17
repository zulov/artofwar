#pragma once
#include "objects/order/FutureOrder.h"
#include "UnitOrder.h"

enum class UnitAction : char;

class IndividualOrder : public UnitOrder {
public:
	IndividualOrder(Unit* unit, UnitAction action,
	                const Urho3D::Vector2& vector, Physical* toUse,
	                bool append = false);

	
	~IndividualOrder();
	bool expired() override;
	bool add() override;
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

	Physical* physical;
};

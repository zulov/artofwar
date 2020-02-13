#pragma once
#include "FutureOrder.h"

class Formation;

class FormationOrder : public FutureOrder
{
public:
	FormationOrder(Formation* formation, UnitOrder action, const Urho3D::Vector2& vector,
	               Physical* toUse, bool append = false);
	~FormationOrder();
	bool add() override;
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

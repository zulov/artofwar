#pragma once
#include "FutureOrder.h"

class Formation;

class FormationOrder : public FutureOrder
{
public:
	FormationOrder(Formation* formation, UnitOrder action, const Urho3D::Vector2& vector,
	               const Physical* physical, bool append = false);
	~FormationOrder();
	bool add() override;
private:
	void addTargetAim() override;
	void addFollowAim() override;
	void addChargeAim() override;
	void addAttackAim() override;
	void addDefendAim() override;
	void simpleAction();
	void addDeadAim() override;
	Formation* formation;
};

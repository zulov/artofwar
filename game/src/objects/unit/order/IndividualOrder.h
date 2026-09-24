#pragma once
#include "UnitOrder.h"

class Unit;
enum class UnitAction : unsigned char;

class IndividualOrder : public UnitOrder {
public:
	IndividualOrder(Unit* unit, UnitAction action, const Urho3D::Vector2& vector, bool append = false);
	IndividualOrder(Unit* unit, UnitAction action, Physical* toUse, bool append = false);
	~IndividualOrder() override = default;

	bool expired() const override;
	bool add() override;
	PendingCommandSaveData saveState(unsigned short order) const override;

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
	void followAndAct();
	std::vector<int> getIndexesToAct() const;

	Unit* unit;
};

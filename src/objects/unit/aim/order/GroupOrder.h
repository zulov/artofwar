#pragma once
#include "FutureOrder.h"
#include "objects/ActionType.h"

class GroupOrder : public FutureOrder {
public:
	GroupOrder(std::vector<Physical*>* entities, UnitOrder action, const Urho3D::Vector2& vector,
	           Physical* toUse, ActionType menuAction, bool append = false);
	~GroupOrder();
	bool add() override;
private:
	const ActionType actionType;
	std::vector<Physical*>* entities;
	//TODO to trzeba kopiowac, ale wtedy trzeba sprawdzac przed wykonaniem
	//czy cos sie nie zepsulo ale i tak cos takiego robie w formacji

	void addCollectAim() override;
	void addTargetAim() override;
	void addFollowAim() override;
	void addChargeAim() override;
	void addAttackAim() override;
	void addDefendAim() override;
	void addDeadAim() override;
	void addStopAim() override;

	void simpleAction(ActionParameter& parameter) const;
	void simpleAction() const;
	void transformToFormationOrder() const;
};

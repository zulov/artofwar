#pragma once
#include "FutureOrder.h"
#include "objects/ActionType.h"

class GroupOrder : public FutureOrder
{
public:
	GroupOrder(std::vector<Physical*>* entities, UnitOrder action, const Urho3D::Vector2& vector,
	           const Physical* physical, ActionType menuAction, bool append = false);
	~GroupOrder();
	bool add() override;
private:
	std::vector<Physical*>* entities;
	const ActionType menuAction;
	//TODO to trzeba kopiowac, ale wtedy trzeba sprawdzac przed wykonaniem czy cos sie nie zepsulo

	void addTargetAim() override;
	void addFollowAim() override;
	void addChargeAim() override;
	void addAttackAim() override;
	void addDefendAim() override;
	void addDeadAim() override;

	void simpleAction(ActionParameter parameter);
	void transformToFormationOrder() const;
};

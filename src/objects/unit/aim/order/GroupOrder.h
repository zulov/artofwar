#pragma once
#include "FutureOrder.h"

class GroupOrder : public FutureOrder
{
public:
	GroupOrder(std::vector<Physical*>* entities, const Urho3D::Vector2& vector, const Physical* physical, UnitOrder action);
	~GroupOrder();
	bool add(bool append) override;
private:
	std::vector<Physical*>* entities;//TODO to trzeba kopiowac, ale wtedy trzeba sprawdzac przed wykonaniem czy cos sie nie zepsulo

	void addTargetAim() override;
	void addFollowAim() override;
	void addChargeAim() override;
	void addAttackAim() override;
	void addDefendAim() override;
	void addDeadAim() override;
	void simpleAction();
};

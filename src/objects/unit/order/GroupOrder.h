#pragma once
#include <vector>

#include "UnitOrder.h"

enum class UnitActionType : char;
enum class UnitAction : char;
class Unit;

class GroupOrder : public UnitOrder {
public:
	void addUnits(const std::vector<Physical*>& entities);
	GroupOrder(const std::vector<Physical*>& entities, UnitActionType actionType, short id,
	           Physical* toUse, bool append = false);

	GroupOrder(const std::vector<Physical*>& entities, UnitActionType actionType, short id,
	           Urho3D::Vector2 vector, bool append = false);

	GroupOrder(std::vector<Unit*> entities, UnitActionType actionType, short id,
	           Urho3D::Vector2 vector, bool append = false);

	~GroupOrder() = default;

	bool add() override;
	bool expired() override;
	short getSize() const override;
private:
	std::vector<Unit*> units;
	//TODO to trzeba kopiowac, ale wtedy trzeba sprawdzac przed wykonaniem
	//czy cos sie nie zepsulo ale i tak cos takiego robie w formacji
	const UnitActionType actionType;
	//bool shouldDivide = false;

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
	void actOnFormation(std::vector<Unit*>& group) const;
	void transformToFormationOrder() const;
};

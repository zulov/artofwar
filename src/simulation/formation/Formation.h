#pragma once
#include <optional>
#include <vector>
#include <Urho3D/Math/Vector2.h>
#include "FormationState.h"
#include "FormationType.h"

class FormationOrder;
class UnitOrder;
class Unit;

class Formation {
public:
	Formation(short _id, const std::vector<Unit*>& _units, FormationType _type, Urho3D::Vector2 _direction);
	~Formation();

	void update();
	void remove();
	Urho3D::Vector2 getPositionFor(short id) const;
	float getPriority(int id) const;
	FormationState getState() const { return state; }
	std::optional<Unit*> getLeader();

	void addOrder(FormationOrder* order);
	size_t getSize() const;
	void semiReset();
	std::vector<Unit*>& getUnits() { return units; }
	void stopAllBesideLeader();
private:
	Urho3D::Vector2 computeLocalCenter();
	void chooseLeader(Urho3D::Vector2& localCenter);
	void updateUnits();
	void setCenter();
	void updateIds();
	void updateSideSize();
	void calculateNotWellFormed();
	void innerUpdate();

	void electLeader();
	void setFormationClearPosition();
	void setPosInFormationForLeader() const;

	void changeState(FormationState newState);
	short id;
	short sideA;
	short sideB;
	FormationType type;
	FormationState state;

	char* levelOfReach;
	float sparsity = 1;
	bool changed = true;

	Urho3D::Vector2 center;
	Urho3D::Vector2 direction;
	Unit* leader = nullptr;
	Unit* oldLeader = nullptr;

	std::vector<Unit*> units;
	std::vector<UnitOrder*> unitOrders;

	float thresholdMin = 0.01f;
	float thresholdMax = 0.5f;
	float notWellFormed = 1.0f;
	float notWellFormedExact = 1.0f;
};

#pragma once
#include "FormationState.h"
#include "FormationType.h"
#include <Urho3D/Math/Vector2.h>
#include <optional>
#include <vector>


class FutureOrder;
class Physical;
class Unit;

class Formation
{
public:
	Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Urho3D::Vector2& _direction);
	~Formation();

	void update();
	void remove();
	Urho3D::Vector2 getPositionFor(short id) const;
	float getPriority(int id) const;
	FormationState getState() const { return state; }
	std::optional<Physical*> getLeader();

	void addOrder(FutureOrder* order);
	size_t getSize();
	void semiReset();
	std::vector<Unit*>& getUnits() { return units; }
	void stopAllBesideLeader();
private:
	Urho3D::Vector2 computeLocalCenter();
	void setNewLeader(Urho3D::Vector2& localCenter);
	void updateUnits();
	void setCenter();
	void updateIds();
	void updateSideSize();
	void calculateNotWellFormed();
	void innerUpdate();

	void electLeader();
	void setFormationClearPosition();
	void setPosInFormationForLeader();

	void changeState(FormationState newState);
	short id;
	short sideA;
	short sideB;
	FormationType type;
	bool changed = true;

	FormationState state;
	float sparsity = 1;

	char* levelOfReach;

	Urho3D::Vector2 center;
	Urho3D::Vector2 direction;
	Unit* leader = nullptr;
	Unit* oldLeader = nullptr;

	std::vector<Unit*> units;
	std::vector<FutureOrder*> futureOrders;

	float thresholdMin = 0.01;
	float thresholdMax = 0.5;
	float notWellFormed = 1.0;
	float notWellFormedExact = 1.0;
};

#pragma once
#include <optional>
#include <unordered_map>
#include <vector>
#include <Urho3D/Math/Vector2.h>
#include "FormationState.h"
#include "FormationType.h"

class FormationOrder;
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
	bool isLeader(Unit* unit) const;
	bool isMoving(Unit* unit) const;
	int getCachePath(int startIdx, int aimIndex) const;
	void addCachePath(int startIdx, int aimIndex, int next);
private:
	bool hasLeader() const;

	void chooseLeader(Urho3D::Vector2& localCenter);
	void updateUnits();
	void setCenter();
	void updateIds();
	void updateSideSize();
	void calculateCohesion();
	void innerUpdate();
	void removePending();

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
	std::vector<FormationOrder*> unitOrders;
	FormationOrder* pendingOrder{};

	float notWellFormed = 1.f;
	float notWellFormedExact = 1.f;

	std::unordered_map<size_t, int> pathCache;
};

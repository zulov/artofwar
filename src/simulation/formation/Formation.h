#pragma once
#include "FormationState.h"
#include "FormationType.h"
#include "UnitOrder.h"
#include <optional>
#include <vector>
#include <Urho3D/Math/Vector2.h>


namespace Urho3D {
	class Vector2;
}

struct FutureAim;
class Physical;
class Unit;

class Formation
{
public:
	Formation(short _id, std::vector<Physical*>* _units, FormationType _type, Urho3D::Vector2& _direction);
	~Formation();

	void update();
	void remove();
	Urho3D::Vector2 getPositionFor(short id);
	float getPriority(int id) const;
	FormationState getState() const { return state; }
	std::optional<Physical*> getLeader();

	void addAim(const Urho3D::Vector2& _futureTarget, const Physical* _physical, UnitOrder _action, bool append);
	size_t getSize();
	void semiReset();
	std::vector<Unit*>& getUnits() { return units; }
private:
	Urho3D::Vector2 computeLocalCenter();
	void setNewLeader(Urho3D::Vector2& localCenter);
	void updateUnits();
	void updateCenter();
	void updateIds();
	void updateSizes();
	void calculateNotWellFormed();
	void innerUpdate();
	void electLeader();

	void changeState(FormationState newState);
	short id;
	short sideA;
	short sideB;
	float sparsity = 1;

	std::vector<Unit*> units;
	char* rechnessLevel;
	FormationType type;
	Urho3D::Vector2 center;

	bool changed = true;

	Urho3D::Vector2 direction;
	Unit* leader = nullptr;
	Unit* oldLeader = nullptr;
	FormationState state;

	//Aims aims;
	std::vector<FutureAim> futureOrders;
	//bool hasFutureOrder = false;

	float theresholedMin = 0.01;
	float theresholedMax = 0.5;
	float notWellFormed = 1.0;
	float notWellFormedExact = 1.0;
};

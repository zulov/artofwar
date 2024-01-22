#pragma once

#include "ForceStats.h"
#include <vector>

enum class UnitState : char;
class Unit;
class Physical;

class Force {
public:
	Force() = default;
	~Force() = default;

	void separationObstacle(Urho3D::Vector2& newForce, Unit* unit);
	void separationUnits(Urho3D::Vector2& newForce, Unit* unit, std::vector<Physical*>* neights);
	void destOrFormation(Urho3D::Vector2& newForce, Unit* unit);
	void destination(Urho3D::Vector2& newForce, Unit* unit, float factor);
	void formation(Urho3D::Vector2& newForce, Unit* unit);
	void escapeFromInvalidPosition(Urho3D::Vector2& newForce, Unit* unit);
	void inCell(Urho3D::Vector2& newForce, Unit* unit) const;

	ForceStats& stats();
	void changeCoef(int i, int wheel);

private:
	void turnIfAreOpposite(const Urho3D::Vector2& newForce, Urho3D::Vector2& destForce) const;
	float calculateCoef(float distance, float minDist) const;
	void randSepForce(Urho3D::Vector2& newForce) const;
	float boostCoef = 100;

	float sepCoef = 10;
	float aimCoef = 2;
	float escapeCoef = 20;
	float formationCoef = 50;
	float inCellCoef = 1;
	float inSocketCoef = 10;

	ForceStats forceStats;
};

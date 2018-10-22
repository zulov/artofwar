#pragma once

#include "ForceStats.h"
#include <vector>
#include <Urho3D/Math/Vector3.h>

class Unit;
class Physical;

class Force
{
public:
	Force();
	~Force();

	void separationObstacle(Urho3D::Vector2& newForce, Unit* unit);
	void separationUnits(Urho3D::Vector2& newForce, Unit* unit, std::vector<Physical *>* units);
	void destination(Urho3D::Vector2& newForce, Unit* unit);
	void formation(Urho3D::Vector2& newForce, Unit* unit);
	void escapeFromInvalidPosition(Urho3D::Vector2& newForce, Unit* unit);
	void inCell(Urho3D::Vector2& newForce, Unit* unit);
	void inSocket(Urho3D::Vector2& newForce, Unit* unit);

	ForceStats& stats();
	void changeCoef(int i, int wheel);

private:
	float calculateCoef(float distance, float minDist) const;

	float boostCoef = 100;

	float sepCoef = 10;
	float aimCoef = 2;
	float escapeCoef = 20;
	float formationCoef = 50;
	float inCellCoef = 1;
	float inSocketCoef = 10;

	ForceStats forceStats;
};

#pragma once

#include "ForceStats.h"
#include <vector>

class Unit;
class Physical;

class Force
{
public:
	Force();
	~Force();

	void separationObstacle(Urho3D::Vector2& newForce, Unit* unit, const Urho3D::Vector2& repulse);
	void separationUnits(Urho3D::Vector2& newForce, Unit* unit, std::vector<Physical *>* units);
	void destination(Urho3D::Vector2& newForce, Unit* unit);
	void formation(Urho3D::Vector2& newForce, Unit* unit);
	void escapeFromInvalidPosition(Urho3D::Vector2& newForce, Urho3D::Vector2* dir);
	float* stats();
	void changeCoef(int i, int wheel);

private:
	float calculateCoef(float distance, float minDist) const;

	float boostCoef = 100;

	float sepCoef = 10;
	float aimCoef = 2;
	float escapeCoef = 20;
	float formationCoef = 50;

	ForceStats forceStats;
};

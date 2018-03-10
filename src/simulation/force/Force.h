#pragma once

#include "objects/unit/Unit.h"
#include <vector>

class Force
{
public:
	Force();
	~Force();

	Vector3* separationObstacle(Unit* unit, const Vector2& repulse);
	Vector3* separationUnits(Unit* unit, std::vector<Unit *>* units);
	Vector3* destination(Unit* unit);
	Vector3* cohesion(Unit* unit, std::vector<Unit*>* units);

private:
	float calculateCoef(double distance, double minDist);
	float boostCoef = 10;
	float sepCoef = 0.5;
	float aimCoef = 2;
	float cohCoef = 1;
};

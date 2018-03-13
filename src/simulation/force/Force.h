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
	Vector3* formation(Unit* unit);
	Vector3* escapeFromInvalidPosition(Vector3* dir);

private:
	float calculateCoef(double distance, double minDist);
	float boostCoef = 10;
	float sepCoef = 0.15;
	float aimCoef = 2;
	float escapeCoef = 200;

	float formationCoef = 1;
};

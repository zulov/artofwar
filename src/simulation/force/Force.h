#pragma once

#include "objects/unit/Unit.h"
#include <vector>

class Force
{
public:
	Force();
	~Force();

	void separationObstacle(Vector2& newForce, Unit* unit, const Vector2& repulse);
	void separationUnits(Vector2& newForce, Unit* unit, std::vector<Unit *>* units);
	void destination(Vector2& newForce, Unit* unit);
	void formation(Vector2& newForce, Unit* unit);
	void escapeFromInvalidPosition(Vector2& newForce, Vector3* dir);

private:
	float calculateCoef(double distance, double minDist);
	float boostCoef = 10;

	float sepCoef = 0.15;
	float aimCoef = 2;
	float escapeCoef = 200;
	float formationCoef = 1;
};

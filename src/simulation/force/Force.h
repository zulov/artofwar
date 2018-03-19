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
	void destination(Vector2& newForce, Unit* unit) const;
	void formation(Vector2& newForce, Unit* unit) const;
	void escapeFromInvalidPosition(Vector2& newForce, Vector2* dir) const;

private:
	float calculateCoef(float distance, float minDist) const;
	float boostCoef = 10;

	float sepCoef = 0.5;
	float aimCoef = 2;
	float escapeCoef = 200;
	float formationCoef = 1;
};

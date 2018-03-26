#pragma once

#include "ForceStats.h"
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
	void escapeFromInvalidPosition(Vector2& newForce, Vector2* dir);
	float* stats();

private:
	float calculateCoef(float distance, float minDist) const;

	float boostCoef = 100;

	float sepCoef = 1;
	float aimCoef = 2;
	float escapeCoef = 20;
	float formationCoef = 1;

	ForceStats forceStats;
};

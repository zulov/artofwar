#pragma once

#include <vector>
#include "objects/unit/Unit.h"

class Force {

public:
	Force();
	~Force();
	Urho3D::Vector3* separationObstacle(Unit* unit, Vector2& repulse);

	Urho3D::Vector3 *separationUnits(Unit * unit, std::vector<Unit *> *units);
	Urho3D::Vector3 *destination(Unit * unit);
private:
	const float coef = 1;
	float calculateCoef(double distance, double minDist);
	float boostCoef = 10;
	float sepCoef = 1;
	float aimCoef = 2;
};


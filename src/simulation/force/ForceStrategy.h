#pragma once

#include <vector>
#include "objects/unit/Unit.h"

class ForceStrategy {

public:
	ForceStrategy();
	~ForceStrategy();
	Urho3D::Vector3* separationObstacle(Unit* unit, Vector3* repulse);

	Urho3D::Vector3 *separationObstacle(Unit * unit, std::vector<Physical *> *obstacles);
	Urho3D::Vector3 *separationUnits(Unit * unit, std::vector<Physical *> *units);
	Urho3D::Vector3 *destination(Unit * unit);
private:
	const double coef = 1;
	double calculateCoef(double distance, double minDist);
	double boostCoef = 10;
	double sepCoef = 1;
	double aimCoef = 2;
};


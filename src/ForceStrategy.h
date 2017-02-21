#pragma once

#include <vector>
#include "Unit.h"

class ForceStrategy {

public:
	ForceStrategy();
	~ForceStrategy();

	Urho3D::Vector3 separationObstacle(Unit * unit, void* param2);
	Urho3D::Vector3 separationUnits(Unit * unit, std::vector<Unit *> *units);
	Urho3D::Vector3 randomForce();
private:
	const double coef = 1;
	double calculateCoef(double distance, double minDist);
	double boostCoef = 10;
};


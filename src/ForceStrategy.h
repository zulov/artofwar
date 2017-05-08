#pragma once

#include <vector>
#include "Unit.h"

class ForceStrategy {

public:
	ForceStrategy();
	~ForceStrategy();

	Urho3D::Vector3 *separationObstacle(Unit * unit, std::vector<Entity *> *obstacles);
	Urho3D::Vector3 *separationUnits(Unit * unit, std::vector<Entity *> *units);
	Urho3D::Vector3 *destination(Unit * unit);
	Urho3D::Vector3 *randomForce();
private:
	const double coef = 1;
	double calculateCoef(double distance, double minDist);
	double boostCoef = 10;
	double sepCoef = 1;
	double aimCoef = 2;
};


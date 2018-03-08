#include "Force.h"

Force::Force() = default;


Force::~Force() = default;

Vector3* Force::separationObstacle(Unit* unit, Vector2& repulse) {
	Vector3* force = new Vector3(repulse.x_, 0, repulse.y_);

	*force *= coef * boostCoef * sepCoef;
	return force;
}

Vector3* Force::separationUnits(Unit* unit, std::vector<Unit*>* units) {
	Vector3* force = new Vector3();

	for (auto neight : *units) {
		float sqSepDist = unit->getMaxSeparationDistance() + neight->getMinimalDistance();
		sqSepDist *= sqSepDist;

		Vector3 diff = *unit->getPosition() - *neight->getPosition();
		const float sqDistance = diff.LengthSquared();
		if (sqDistance > sqSepDist) { continue; }
		if (sqDistance == 0) {
			force->x_ = (static_cast<double>(rand()) / (RAND_MAX)) * coef - (coef / 2);
			force->z_ = (static_cast<double>(rand()) / (RAND_MAX)) * coef - (coef / 2);
			*force *= boostCoef * sepCoef;
			return force;
		}
		const float distance = sqrt(sqDistance);

		diff /= distance;
		const float minimalDistance = unit->getMinimalDistance() + neight->getMinimalDistance();
		const float coef = calculateCoef(distance, minimalDistance);

		diff *= coef;
		*force += diff;
	}

	*force *= boostCoef * sepCoef;
	return force;
}

Vector3* Force::destination(Unit* unit) {
	return unit->getDestination(boostCoef, aimCoef);
}

float Force::calculateCoef(double distance, double minDist) {
	double parameter = distance - minDist / 2;
	if (parameter <= 0.05) {
		parameter = 0.05;
	}
	return exp(minDist / (distance + 0.05)) + exp(1 / parameter) - 2;
}

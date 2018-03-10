#include "Force.h"

Force::Force() = default;


Force::~Force() = default;

Vector3* Force::separationObstacle(Unit* unit, const Vector2& repulse) {
	Vector3* force = new Vector3(repulse.x_, 0, repulse.y_);

	*force *= boostCoef * sepCoef;
	return force;
}

Vector3* Force::separationUnits(Unit* unit, std::vector<Unit*>* units) {
	Vector3* force = new Vector3();
	if (units->empty()) {
		return force;
	}

	for (auto neight : *units) {
		float sqSepDist = unit->getMaxSeparationDistance() + neight->getMinimalDistance();
		sqSepDist *= sqSepDist;

		Vector3 diff = *unit->getPosition() - *neight->getPosition();
		const float sqDistance = diff.LengthSquared();
		if (sqDistance > sqSepDist) { continue; }
		if (sqDistance == 0) {
			force->x_ = static_cast<double>(rand()) / RAND_MAX - 0.5;
			force->z_ = static_cast<double>(rand()) / RAND_MAX - 0.5;
			*force *= boostCoef * sepCoef;
			return force;
		}
		const float distance = sqrt(sqDistance);

		const float minimalDistance = unit->getMinimalDistance() + neight->getMinimalDistance();
		const float coef = calculateCoef(distance, minimalDistance);

		diff *= coef / distance;

		*force += diff;
	}

	*force *= boostCoef * sepCoef;
	return force;
}

Vector3* Force::destination(Unit* unit) {
	return unit->getDestination(boostCoef, aimCoef);
}

Vector3* Force::cohesion(Unit* unit, std::vector<Unit*>* units) {
	Vector3* force = new Vector3();

	float k = 0;
	for (auto neight : *units) {
		Vector3 diff = *unit->getPosition() - *neight->getPosition();
		const float sqDistance = diff.LengthSquared();
		const float minimalDistance = unit->getMinimalDistance() + neight->getMinimalDistance();

		const float weight = getWeight(unit, neight);
		if (sqDistance > minimalDistance * minimalDistance) {
			*force += *neight->getPosition() * weight;
			k += weight;
		}
	}
	if (k > 0) {
		*force /= k;
		*force -= *unit->getPosition();

		*force *= boostCoef * cohCoef;
	}
	return force;
}

float Force::getWeight(Unit* unit, Unit* neight) {
	if (unit->getDbID() == neight->getDbID()) {
		return 1;
	} else {
		return 0.3;
	}
}

float Force::calculateCoef(double distance, double minDist) {
	return exp(minDist / (distance + 0.05)) - 1;
}

#include "Force.h"

Force::Force() {
}


Force::~Force() {
}

Urho3D::Vector3* Force::separationObstacle(Unit* unit, Vector2& repulse) {
	Vector3* force = new Vector3(repulse.x_, 0, repulse.y_);

	(*force) *= coef * boostCoef * sepCoef;
	return force;
}

Urho3D::Vector3* Force::separationUnits(Unit* unit, std::vector<Unit*>* units) {
	Vector3* force = new Vector3();

	for (int i = 0; i < units->size(); ++i) {
		Unit* neight = (*units)[i];
		double sqSepDist = unit->getMaxSeparationDistance() + neight->getMinimalDistance();
		sqSepDist *= sqSepDist;

		Vector3 diff = *unit->getPosition() - *neight->getPosition();
		double sqDistance = diff.LengthSquared();
		if (sqDistance > sqSepDist) { continue; }
		if (sqDistance == 0) {
			force->x_ = ((double)rand() / (RAND_MAX)) * coef - (coef / 2);
			force->z_ = ((double)rand() / (RAND_MAX)) * coef - (coef / 2);
			(*force) *= boostCoef * sepCoef;
			return force;
		}
		double distance = sqrt(sqDistance);

		diff /= distance;
		double minimalDistance = unit->getMinimalDistance() + neight->getMinimalDistance();
		double coef = calculateCoef(distance, minimalDistance);//poprawic kolejnosc zeby ograniczyc operacje na vektorze

		diff *= coef;
		(*force) += diff;
	}

	(*force) *= boostCoef * sepCoef;
	return force;
}

Urho3D::Vector3* Force::destination(Unit* unit) {
	return unit->getDestination(boostCoef, aimCoef);
}

double Force::calculateCoef(double distance, double minDist) {
	double parameter = distance - minDist / 2;
	if (parameter <= 0.05) {
		parameter = 0.05;
	}
	return exp(minDist / (distance + 0.05)) + exp(1 / parameter) - 2;
}

#include "ForceStrategy.h"

ForceStrategy::ForceStrategy() {
}


ForceStrategy::~ForceStrategy() {
}

Urho3D::Vector3* ForceStrategy::separationObstacle(Unit* unit, Vector3* repulse) {
	Vector3* force = new Vector3(*repulse);


	(*force) *= coef*boostCoef * sepCoef;
	return force;
}

Urho3D::Vector3* ForceStrategy::separationObstacle(Unit* unit, std::vector<Entity*>* obstacles) {
	return separationUnits(unit, obstacles);
}


Urho3D::Vector3* ForceStrategy::separationUnits(Unit* unit, std::vector<Entity*>* units) {
	Vector3* force = new Vector3();

	for (int i = 0; i < units->size(); ++i) {
		Entity* neight = (*units)[i];
		double sqSepDist = unit->getMaxSeparationDistance() + neight->getMinimalDistance();
		sqSepDist *= sqSepDist;

		Vector3 diff = *unit->getPosition() - *neight->getPosition();
		double sqDistance = diff.LengthSquared();
		if (sqDistance > sqSepDist) { continue; }
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

Urho3D::Vector3* ForceStrategy::destination(Unit* unit) {
	Vector3* aim = unit->getAim();
	if (aim != nullptr) {
		Vector3* force = new Vector3((*(aim)) - (*unit->getPosition()));
		force->Normalize();
		(*force) *= boostCoef;
		(*force) -= (*unit->getVelocity());
		(*force) /= 0.5;
		(*force) *= unit->getMass();
		(*force) *= aimCoef;
		return force;
	}
	return new Vector3();
}

Urho3D::Vector3* ForceStrategy::randomForce() {
	//double x = ((double)rand() / (RAND_MAX)) * coef - (coef / 2);
	//double y = ((double)rand() / (RAND_MAX)) * coef - (coef / 2);
	//return Urho3D::Vector3(x, 0, y);
	return new Vector3();
}

double ForceStrategy::calculateCoef(double distance, double minDist) {
	double parameter = distance - minDist;
	if (parameter <= 0.05) {//zapobieganie nieskonczonosci
		parameter = 0.05;
	}
	double coef = exp(1 / parameter) - 1;

	return coef;
}

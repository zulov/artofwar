#include "ForceStrategy.h"



ForceStrategy::ForceStrategy() {}


ForceStrategy::~ForceStrategy() {}

Urho3D::Vector3 *ForceStrategy::separationObstacle(Unit * unit, void* param2) {
	return new Vector3();
}

Urho3D::Vector3 *ForceStrategy::separationUnits(Unit * unit, std::vector<Unit*> *units) {
	Vector3 *force = new Vector3();

	for (int i = 0; i < units->size(); ++i) {
		double distance = ((*units)[i]->getPosition() - unit->getPosition()).Length();
		if (distance > unit->getMaxSeparationDistance()) { continue; }

		Urho3D::Vector3 newForce = unit->getPosition() - (*units)[i]->getPosition();
		newForce /= distance;
		double minimalDistance = (unit->getMinimalDistance() + (*units)[i]->getMinimalDistance());
		double coef = calculateCoef(distance, minimalDistance);

		newForce *= coef;
		(*force) += newForce;
	}

	(*force) *= boostCoef;
	return force;
}

Urho3D::Vector3 *ForceStrategy::destination(Unit* unit) {
	Vector3 *force = new Vector3();
	if (unit->getAim() != nullptr) {
		force = new Vector3((*(unit->getAim())) - unit->getPosition());
		force->Normalize();
		(*force) *= boostCoef;
		(*force) -= unit->getVelocity();
		(*force) /= 0.5;
		(*force) *= unit->getMass();
	}
	return force;
}

Urho3D::Vector3 *ForceStrategy::randomForce() {
	//double x = ((double)rand() / (RAND_MAX)) * coef - (coef / 2);
	//double y = ((double)rand() / (RAND_MAX)) * coef - (coef / 2);
	//return Urho3D::Vector3(x, 0, y);
	return new Vector3(0, 0, 0);
}

double ForceStrategy::calculateCoef(double distance, double minDist) {
	double parameter = distance - minDist;
	if (parameter <= 0.04) {//zapobieganie nieskonczonosci
		parameter = 0.04;
	}
	double coef = exp((minDist) / parameter) - 1;
	//coef *= sepCoefWall;

	return coef;
}

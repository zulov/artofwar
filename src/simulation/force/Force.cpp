#include "Force.h"
#include "Game.h"

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

Vector3* Force::formation(Unit* unit) {
	auto opt = Game::get()->getFormationManager()->getPositionFor(unit);
	if (opt.has_value()) {
		float wellFormed = Game::get()->getFormationManager()->getWellFormed(unit);
		auto force = new Vector3(opt.value() - *unit->getPosition());
		*force *= formationCoef * boostCoef * (1 - wellFormed);
		return force;
	}
	return new Vector3();
}

Vector3* Force::escapeFromInvalidPosition(Vector3* dir) {
	*dir *= escapeCoef;
	return dir;
}

float Force::calculateCoef(double distance, double minDist) {
	return exp(minDist / (distance + 0.05)) - 1;
}

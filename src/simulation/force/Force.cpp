#include "Force.h"
#include "Game.h"

Force::Force() = default;


Force::~Force() = default;

void Force::separationObstacle(Vector2& newForce, Unit* unit, const Vector2& repulse) {
	if (repulse == Vector2::ZERO) {
		return;
	}
	Vector2 force(repulse.x_, repulse.y_);
	const auto distance = force.Length();
	const auto minimalDistance = unit->getMinimalDistance() * 3;
	const auto coef = calculateCoef(distance, minimalDistance);

	force *= coef / distance;
	force *= boostCoef * sepCoef;
	newForce += force;
}

void Force::separationUnits(Vector2& newForce, Unit* unit, std::vector<Unit*>* units) {
	if (units->empty()) {
		return;
	}
	Vector2 force;

	for (auto neight : *units) {
		float sqSepDist = unit->getMaxSeparationDistance() + neight->getMinimalDistance();
		sqSepDist *= sqSepDist;

		Vector3 diff = *unit->getPosition() - *neight->getPosition();
		const float sqDistance = diff.LengthSquared();
		if (sqDistance > sqSepDist) { continue; }
		if (sqDistance == 0) {
			force.x_ = static_cast<double>(rand()) / RAND_MAX - 0.5;
			force.y_ = static_cast<double>(rand()) / RAND_MAX - 0.5;
			force *= boostCoef * sepCoef;
			newForce += force;
			return;
		}
		const float distance = sqrt(sqDistance);

		const float minimalDistance = unit->getMinimalDistance() + neight->getMinimalDistance();
		const float coef = calculateCoef(distance, minimalDistance);

		diff *= coef / distance;
		force.x_ = diff.x_;
		force.y_ = diff.z_;
	}

	force *= boostCoef * sepCoef;
	newForce += force;
}

void Force::destination(Vector2& newForce, Unit* unit) const {
	const auto force = unit->getDestination(boostCoef, aimCoef);
	newForce+=force;
}

void Force::formation(Vector2& newForce, Unit* unit) const {
	auto opt = Game::get()->getFormationManager()->getPositionFor(unit);
	if (opt.has_value()) {
		const float wellFormed = Game::get()->getFormationManager()->getWellFormed(unit);
		const auto force = Vector3(opt.value() - *unit->getPosition()) * formationCoef * boostCoef * (1 - wellFormed);
		newForce.x_ += force.x_;
		newForce.y_ += force.z_;
	}
}

void Force::escapeFromInvalidPosition(Vector2& newForce, Vector3* dir) const {
	newForce.x_ += dir->x_ * escapeCoef;
	newForce.y_ += dir->z_ * escapeCoef;
	delete dir;
}

float Force::calculateCoef(const float distance, const float minDist) const {
	return exp(minDist / (distance + 0.05)) - 1;
}

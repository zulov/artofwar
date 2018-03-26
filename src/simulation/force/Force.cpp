#include "Force.h"
#include "Game.h"
#include <algorithm>

Force::Force() {
}


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

	forceStats.addSepObst(force);

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

		Vector2 diff(
		             unit->getPosition()->x_ - neight->getPosition()->x_,
		             unit->getPosition()->z_ - neight->getPosition()->z_
		            );
		const float sqDistance = diff.LengthSquared();
		if (sqDistance > sqSepDist) { continue; }
		if (sqDistance == 0) {
			force.x_ = static_cast<float>(rand()) / RAND_MAX - 0.5;
			force.y_ = static_cast<float>(rand()) / RAND_MAX - 0.5;
			force *= boostCoef * sepCoef;
			newForce += force;
			return;
		}
		const float distance = sqrt(sqDistance);

		const float minimalDistance = unit->getMinimalDistance() + neight->getMinimalDistance();
		const float coef = calculateCoef(distance, minimalDistance);

		diff *= coef / distance;
		force += diff;
	}

	force *= boostCoef * sepCoef;

	forceStats.addSepUnit(force);

	newForce += force;
}

void Force::destination(Vector2& newForce, Unit* unit) {
	auto force = unit->getDestination(boostCoef, aimCoef);

	forceStats.addDest(force);

	newForce += force;
}

void Force::formation(Vector2& newForce, Unit* unit) {
	auto opt = Game::get()->getFormationManager()->getPositionFor(unit);
	if (opt.has_value()) {
		const float wellFormed = Game::get()->getFormationManager()->getWellFormed(unit);
		auto force = Vector2(
		                     opt.value().x_ - unit->getPosition()->x_,
		                     opt.value().y_ - unit->getPosition()->z_
		                    );
		force *= formationCoef * boostCoef * (1 - wellFormed);

		forceStats.addForm(force);

		newForce += force;
	}
}

void Force::escapeFromInvalidPosition(Vector2& newForce, Vector2* dir) {
	if (dir) {
		auto force = *dir * escapeCoef * boostCoef;

		forceStats.addEscp(force);

		newForce += force;
		delete dir;
	}
}

float* Force::stats() {
	return forceStats.result();
}

float Force::calculateCoef(const float distance, const float minDist) const {
	float parameter = distance - minDist / 2;
	float coef;
	if (distance < minDist) {
		coef = -10 * distance + 25;
	} else {
		coef = 0;
	}
	if (parameter <= 0.05) {
		parameter = 0.05;
	}
	return 1 / parameter + coef;
}

//float Force::calculateCoef(const float distance, const float minDist) const {
//	return exp(minDist / (distance + 0.05)) - 1;
//}

//float Force::calculateCoef(const float distance, const float minDist) const {
//	double parameter = distance - minDist/2;
//	if (parameter <= 0.05) {
//		parameter = 0.05;
//	}
//	return exp(1 / parameter) - 1;
//}

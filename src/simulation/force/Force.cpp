#include "Force.h"
#include <Urho3D/IO/Log.h>
#include "Game.h"
#include "math/MathUtils.h"

#include "math/RandGen.h"
#include "objects/unit/Unit.h"
#include "simulation/env/Environment.h"
#include "simulation/formation/FormationManager.h"


void Force::separationObstacle(Urho3D::Vector2& newForce, Unit* unit) {
	auto force = Game::getEnvironment()->repulseObstacle(unit);
	if (force == Urho3D::Vector2::ZERO) {
		return;
	}
	const auto distance = force.Length();
	const auto minimalDistance = unit->getMinimalDistance() * 3;
	const auto coef = calculateCoef(distance, minimalDistance);

	force *= coef / distance;
	force *= boostCoef * sepCoef;

	forceStats.addSepObst(force);

	newForce += force;
}

void Force::randSepForce(Urho3D::Vector2& newForce) {
	Urho3D::Vector2 force;
	force.x_ = RandGen::nextRand(RandFloatType::COLLISION_FORCE, 1.f) - 0.5f;
	force.y_ = RandGen::nextRand(RandFloatType::COLLISION_FORCE, 1.f) - 0.5f;
	force *= boostCoef * sepCoef;
	newForce += force;
}

void Force::separationUnits(Urho3D::Vector2& newForce, Unit* unit, std::vector<Physical*>* neights) {
	if (neights->empty()) {
		return;
	}
	Urho3D::Vector2 force;
	const int isLeader = Game::getFormationManager()->isLeader(unit);

	for (auto physical : *neights) {
		auto neight = dynamic_cast<Unit*>(physical);
		float sqSepDist = unit->getMaxSeparationDistance() + neight->getMinimalDistance();
		sqSepDist *= sqSepDist;

		auto diff = dirTo(neight->getPosition(), unit->getPosition());

		const float sqDistance = diff.LengthSquared();
		if (sqDistance > sqSepDist
			|| (isLeader != -1 && unit->getFormation() == neight->getFormation())) { continue; }

		if (sqDistance == 0.f) {
			randSepForce(newForce);
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

void Force::destOrFormation(Urho3D::Vector2& newForce, Unit* unit) {
	if (unit->getFormation() < 0 || Game::getFormationManager()->isLeader(unit)) {
		destination(newForce, unit);
	} else {
		//aims.clearExpired();
		formation(newForce, unit);
	}
}

void Force::destination(Urho3D::Vector2& newForce, Unit* unit) {
	auto force = unit->getDestination(boostCoef, aimCoef);

	forceStats.addDest(force);

	newForce += force;
}

void Force::formation(Urho3D::Vector2& newForce, Unit* unit) {
	assert(!unit->hasAim());
	auto posOpt = Game::getFormationManager()->getPositionFor(unit);
	if (posOpt.has_value()) {
		const float priority = Game::getFormationManager()->getPriority(unit);
		if (priority > 0) {
			auto pos = posOpt.value();
			const auto aimIndex = Game::getEnvironment()->getIndex(pos);
			Urho3D::Vector2 force;
			if (Game::getEnvironment()->isInLocalArea(unit->getMainCell(), aimIndex)) {
				force = dirTo(unit->getPosition(), pos);
			} else {
				auto* const path = Game::getEnvironment()->findPath(unit->getMainCell(), aimIndex, 64);
				//std::cout << unit->getMainCell() << "||" << aimIndex << "||" << path->size() << std::endl;
				if (!path->empty()) {
					auto center = Game::getEnvironment()->getCenter(path->at(0));
					force = dirTo(unit->getPosition(), center);
				} else {
					// auto dist = sqDist(unit->getPosition(), pos);
					// if (dist > 32 * 32) {
					// 	unit->resetFormation();
					// }
					Game::getLog()->Write(0, "brak drogi w formacji");
				}
			}

			force *= formationCoef * boostCoef * priority;

			forceStats.addForm(force);

			newForce += force;
		}
	}
}

void Force::escapeFromInvalidPosition(Urho3D::Vector2& newForce, Unit* unit) {
	const auto opt = Game::getEnvironment()->validatePosition(unit->getPosition());
	if (opt.has_value()) {
		auto force = opt.value() * escapeCoef * boostCoef;

		forceStats.addEscp(force);

		newForce += force;
	}
}

void Force::inCell(Urho3D::Vector2& newForce, Unit* unit) const {
	auto aim = Game::getEnvironment()->getPositionInBucket(unit);
	auto force = dirTo(unit->getPosition(), aim);
	//TODO czy to normalizacja?
	force *= inCellCoef * boostCoef;
	newForce += force;

}

ForceStats& Force::stats() {
	return forceStats;
}

void Force::changeCoef(int i, int wheel) {
	switch (i) {
	case 0: sepCoef += wheel * 0.2;
		std::cout << "Sep" << sepCoef << std::endl;
		break;
	case 1: aimCoef += wheel * 0.2;
		std::cout << "Aim " << aimCoef << std::endl;
		break;
	case 2: formationCoef += wheel * 0.2;
		std::cout << "Form " << formationCoef << std::endl;
		break;
	case 3: escapeCoef += wheel * 0.2;
		std::cout << "Esc " << escapeCoef << std::endl;
		break;
	default:
		break;
	}
}

float Force::calculateCoef(const float distance, const float minDist) const {
	float parameter = distance - minDist / 2;
	if (parameter <= 0.00005f) {
		parameter = 0.00005f;
	}
	return 1 / parameter;
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

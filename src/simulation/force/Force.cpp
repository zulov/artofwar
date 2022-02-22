#include "Force.h"
#include <Urho3D/IO/Log.h>
#include "Game.h"
#include "database/db_strcut.h"
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

void Force::randSepForce(Urho3D::Vector2& diff) const {
	diff.x_ = RandGen::nextRand(RandFloatType::COLLISION_FORCE, 1.f) - 0.5f;
	diff.y_ = RandGen::nextRand(RandFloatType::COLLISION_FORCE, 1.f) - 0.5f;
	diff.Normalize();
	diff /= 1000;
}

void Force::separationUnits(Urho3D::Vector2& newForce, Unit* unit, std::vector<Physical*>* neights) {
	if (neights->empty()) {
		return;
	}
	Urho3D::Vector2 force;
	const bool isLeader = Game::getFormationManager()->isLeader(unit);

	for (auto physical : *neights) {
		auto neight = dynamic_cast<Unit*>(physical); //TODO perf rzutoac calosc vectoru
		float sqSepDist = unit->getMaxSeparationDistance() + neight->getMinimalDistance();
		sqSepDist *= sqSepDist;

		auto diff = dirTo(neight->getPosition(), unit->getPosition());

		float sqDistance = diff.LengthSquared();

		if (sqDistance > sqSepDist //jezeli za dalko lub jest liderem a sasiad jest z tego samego
			|| (isLeader && unit->getFormation() == neight->getFormation())) { continue; }

		if (sqDistance == 0.f) {
			randSepForce(diff);
			sqDistance = diff.LengthSquared();
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
	if (unit->getFormation() < 0) {
		destination(newForce, unit, 1.f);
	} else if (Game::getFormationManager()->isLeader(unit)) {
		if (Game::getFormationManager()->isMoving(unit)) {
			destination(newForce, unit, 1.0f);
		} else {
			destination(newForce, unit, 0.05f);
		}
	} else {
		//aims.clearExpired();
		formation(newForce, unit);
	}
}

void Force::destination(Urho3D::Vector2& newForce, Unit* unit, float factor) {
	auto force = unit->getDestination(boostCoef, aimCoef * factor);

	forceStats.addDest(force);

	newForce += force;
}

void Force::formation(Urho3D::Vector2& newForce, Unit* unit) {
	assert(!unit->hasAim());
	if(unit->hasAim()) {
		Game::getLog()->Write(3, "ERROR unit still has aims");
	}

	const auto formMng = Game::getFormationManager();

	auto posOpt = formMng->getPositionFor(unit); //TODO czasem jest niepassable
	if (posOpt.has_value()) {
		const float priority = formMng->getPriority(unit);
		if (priority > 0) {
			auto pos = posOpt.value();
			const auto env = Game::getEnvironment();

			const auto aimIndex = env->getIndex(pos);
			Urho3D::Vector2 force;
			if (env->isInLocalArea(unit->getMainGridIndex(), aimIndex)) {
				force = dirTo(unit->getPosition(), pos);
			} else {
				int nextIndex = formMng->getCachePath(unit, aimIndex);
				if (nextIndex >= 0) {
					force = dirTo(unit->getPosition(), env->getCenter(nextIndex));
				} else if (nextIndex == -1) {
					auto* const path = env->findPath(unit->getMainGridIndex(), aimIndex, 64);
					if (!path->empty()) {
						formMng->addCachePath(unit, aimIndex, path->at(0));
						auto center = env->getCenter(path->at(0));
						force = dirTo(unit->getPosition(), center);
					} else {
						formMng->addCachePath(unit, aimIndex, -2);
						Game::getLog()->Write(0, "brak drogi w formacji");
					}
				} else {
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
	const auto opt = Game::getEnvironment()->validatePosition(unit->getMainGridIndex(), unit->getPosition());
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

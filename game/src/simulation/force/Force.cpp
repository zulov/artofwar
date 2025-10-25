#include "Force.h"
#include <Urho3D/IO/Log.h>
#include "Game.h"
#include "database/db_strcut.h"
#include "math/MathUtils.h"
#include "math/RandGen.h"
#include "objects/unit/Unit.h"
#include "env/Environment.h"
#include "simulation/formation/FormationManager.h"

constexpr float CLOSEST_DIST = 0.00005f;

void Force::separationObstacle(Urho3D::Vector2& newForce, Unit* unit) {
	auto force = Game::getEnvironment()->repulseObstacle(unit);
	if (force == Urho3D::Vector2::ZERO) {
		return;
	}
	const auto distance = force.Length();
	const auto minimalDistance = unit->getMinimalDistance() * 3;
	const auto coef = calculateCoef(distance, minimalDistance);

	force *= (coef / distance) * boostCoef * sepCoef;

	forceStats.addSepObst(force);

	newForce += force;
}

void Force::randSepForce(Urho3D::Vector2& diff) const {
	diff.x_ = RandGen::nextRand(RandFloatType::COLLISION_FORCE, 1.f) - 0.5f;
	diff.y_ = RandGen::nextRand(RandFloatType::COLLISION_FORCE, 1.f) - 0.5f;
	scaleTo(diff, CLOSEST_DIST);
}

void Force::separationUnits(Urho3D::Vector2& newForce, Unit* unit, std::vector<Physical*>* neights) {
	if (neights->empty()) {
		return;
	}
	Urho3D::Vector2 force;
	const bool isLeader = Game::getFormationManager()->isLeader(unit);

	const std::span<Unit*> neights2 = std::span((Unit**)neights->data(), neights->size());
	for (const auto neight : neights2) {
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
	//enum Single,leader,information
	if (unit->getFormation() < 0) {//single
		destination(newForce, unit, 1.f);
	} else if (Game::getFormationManager()->isLeader(unit)) {//leader
		const auto factor = Game::getFormationManager()->isMoving(unit) ? 1.0f : 0.05f;

		destination(newForce, unit, factor);
	} else {//information
		formation(newForce, unit);
	}
}

void Force::destination(Urho3D::Vector2& newForce, Unit* unit, float factor) {
	auto destForce = unit->getDestination(boostCoef, aimCoef * factor);

	turnIfAreOpposite(newForce, destForce);

	forceStats.addDest(destForce);

	newForce += destForce;
}

void Force::turnIfAreOpposite(const Urho3D::Vector2& newForce, Urho3D::Vector2& destForce) const {
	if (!destForce.Equals(Urho3D::Vector2::ZERO) && !newForce.Equals(Urho3D::Vector2::ZERO)) {
		const auto dot = destForce.DotProduct(newForce);
		if (dot < 0) {
			if (Urho3D::Equals(dot / (newForce.Length() * destForce.Length()), -1.f)) {
				destForce += destForce.Length() * 0.1f * Urho3D::Vector2(destForce.y_, -destForce.x_);
			}
		}
	}
}

void Force::formation(Urho3D::Vector2& newForce, Unit* unit) {
	assert(!unit->hasAim());
	if (unit->hasAim()) {
		Game::getLog()->Write(3, "ERROR unit still has aims");
	}

	const auto formation = Game::getFormationManager()->getFormation(unit);
	auto aimPos = formation->getPositionFor(unit->getPositionInState());

	const float priority = formation->getPriority(unit->getPositionInState());
	if (priority > 0) {
		const auto env = Game::getEnvironment();

		const auto aimIndex = env->getIndex(aimPos);
		Urho3D::Vector2 force;
		auto startIndex = unit->getMainGridIndex();

		int nextIndexFromCache = formation->getCachePath(startIndex, aimIndex);
		if (nextIndexFromCache >= 0) {
			//ten cache chyba jest lzejszy (moze zmienic na vector) niz ten w path finder ale zle resetowany
			force = dirTo(unit->getPosition(), env->getCenter(nextIndexFromCache));
		} else if (nextIndexFromCache == -1) {
			auto* const path = env->findPath(startIndex, aimIndex);
			if (!path->empty()) {
				int nextIndex = path->at(0);
				if (nextIndex != aimIndex) {
					formation->addCachePath(startIndex, aimIndex, nextIndex);
					force = dirTo(unit->getPosition(), env->getCenter(nextIndex));
				} else {
					//close
					force = dirTo(unit->getPosition(), aimPos);
				}
			} else {
				formation->addCachePath(startIndex, aimIndex, -2);
				Game::getLog()->Write(0, "brak drogi w formacji");
				return;
			}
		} else {
			Game::getLog()->Write(0, "brak drogi w formacji");
			return;
		}

		force *= formationCoef * boostCoef * priority;

		turnIfAreOpposite(newForce, force);

		forceStats.addForm(force);

		newForce += force;
	}
}

bool Force::escapeFromInvalidPosition(Urho3D::Vector2& newForce, const Unit* unit) {
	const auto opt = Game::getEnvironment()->validatePosition(unit->getMainGridIndex(), unit->getPosition());
	if (opt.has_value()) {
		auto force = opt.value() * escapeCoef * boostCoef;

		forceStats.addEscp(force);

		newForce += force;
	}
	return Game::getEnvironment()->getOccupationLevel(unit->getMainGridIndex()) > 1;
}

void Force::inCell(Urho3D::Vector2& newForce, Unit* unit) const {
	auto force = dirTo(unit->getPosition(), unit->setInCellPos());
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
	if (parameter <= CLOSEST_DIST) {
		parameter = CLOSEST_DIST;
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

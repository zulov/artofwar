#include "Unit.h"
#include "DebugUnitType.h"
#include "Game.h"
#include "ObjectEnums.h"
#include "UnitOrder.h"
#include "aim/FutureAim.h"
#include "colors/ColorPeletteRepo.h"
#include "database/DatabaseCache.h"
#include "objects/unit/ChargeData.h"
#include "objects/unit/ColorMode.h"
#include "objects/unit/MissleData.h"
#include "scene/load/dbload_container.h"
#include "simulation/force/ForceStats.h"
#include "simulation/formation/FormationManager.h"
#include "state/StateManager.h"
#include "DebugLineRepo.h"
#include <string>
#include <algorithm>


Unit::Unit(Urho3D::Vector3* _position, int id, int player, int level) : Physical(_position, ObjectType::UNIT),
	state(UnitState::STOP) {

	dbUnit = Game::getDatabaseCache()->getUnit(id);
	dbLevel = Game::getDatabaseCache()->getUnitLevel(id, level).value();
	loadXml("Objects/units/" + dbLevel->nodeName);

	basic = model->GetMaterial(0);
	if (StateManager::validateState(getDbID(), UnitState::CHARGE)) {
		chargeData = new ChargeData(150, 2);
	}

	if (StateManager::validateState(getDbID(), UnitState::SHOT)) {
		missleData = new MissleData(150, 2);
	}

	setPlayerAndTeam(player);

	for (auto& bucket : teamBucketIndex) {
		bucket = INT_MIN;
	}
	std::fill_n(useSockets, USE_SOCKETS_NUMBER, false);
}

Unit::~Unit() {
	delete chargeData;
	delete missleData;
}

bool Unit::isAlive() const {
	return state != UnitState::DEAD && state != UnitState::DISPOSE;
}

void Unit::populate() {
	maxSeparationDistance = dbLevel->maxSep;
	mass = dbLevel->mass;
	maxSpeed = dbLevel->maxSpeed;
	minSpeed = dbLevel->minSpeed;
	minimalDistance = dbLevel->minDist;
	attackRange = dbLevel->attackRange;
	attackInterest = dbLevel->attackRange * 10;
	rotatable = dbUnit->rotatable;
	actionState = UnitState(dbUnit->actionState);

	collectSpeed = dbLevel->collectSpeed;

	hpCoef = dbLevel->maxHp;
	maxHpCoef = dbLevel->maxHp;
	attackCoef = dbLevel->attack;
	defenseCoef = dbLevel->defense;
	attackSpeed = dbLevel->attackSpeed / 100.f;
}

void Unit::checkAim() {
	if (aims.ifReach(this)) {
		StateManager::changeState(this, UnitState::MOVE);
	}
}

void Unit::move(double timeStep) const {
	if (state != UnitState::STOP) {
		position->x_ += velocity.x_ * timeStep;
		position->z_ += velocity.y_ * timeStep;
		node->SetPosition(*position);
		//node->Translate((*velocity) * timeStep, TS_WORLD);
	}
	if (missleData && missleData->isUp()) {
		missleData->update(timeStep, attackCoef);
	}
}

void Unit::setAcceleration(Urho3D::Vector2& _acceleration) {
	acceleration = _acceleration;
	if (acceleration.LengthSquared() > dbLevel->maxForce * dbLevel->maxForce) {
		acceleration.Normalize();
		acceleration *= dbLevel->maxForce;
	}
}

void Unit::forceGo(float boostCoef, float aimCoef, Urho3D::Vector2& force) const {
	force.Normalize();
	force *= boostCoef;
	force -= velocity;
	force /= 0.5;
	force *= mass;
	force *= aimCoef;
}

Urho3D::Vector2 Unit::getDestination(float boostCoef, float aimCoef) {
	Urho3D::Vector2 force;
	aims.clearExpired();
	if (aims.hasAim()) {
		auto dirOpt = aims.getDirection(this);
		if (dirOpt.has_value()) {
			force = dirOpt.value();
			forceGo(boostCoef, aimCoef, force);
		}
	}

	return force;
}

void Unit::absorbAttack(float attackCoef) {
	hpCoef -= attackCoef * (1 - defenseCoef);

	updateHealthBar();

	if (hpCoef < 0) {
		StateManager::changeState(this, UnitState::DEAD);
	}
}

void Unit::actionIfCloseEnough(UnitState action, Physical* closest, int indexToInteract,
                               float sqDistance, float closeRange, float intrestRange) {
	if (closest) {
		if (sqDistance < closeRange * closeRange) {
			interactWithOne(closest, indexToInteract, action);
		} else if (sqDistance < intrestRange * intrestRange) {
			addAim(FutureAim(closest, UnitOrder::FOLLOW));
		}
	}
}

void Unit::toAction(Physical* closest, float minDistance, int indexToInteract, UnitState stateTo) {
	actionIfCloseEnough(stateTo, closest, indexToInteract, minDistance, attackRange, attackInterest);
}

void Unit::toAction(Physical* closest, float minDistance, int indexToInteract, UnitState stateTo,
                    float attackInterest) {
	actionIfCloseEnough(stateTo, closest, indexToInteract, minDistance, attackRange, attackInterest);
}

void Unit::interactWithOne(Physical* thing, int indexToInteract, UnitState action) {
	thingsToInteract.clear();
	thingsToInteract.push_back(thing);

	this->indexToInteract = indexToInteract;

	if (!StateManager::changeState(this, action)) {
		thingsToInteract.clear();
	}
}

void Unit::toCharge(std::vector<Physical*>* enemies) {
	thingsToInteract.clear();
	for (auto entity : *enemies) {
		if (entity->isAlive() && sqDist(getPosition(), entity->getPosition()) <= chargeData->attackRange *
			chargeData->attackRange) {
			thingsToInteract.push_back(entity);
		}
	}
}

void Unit::updateHeight(float y, double timeStep) {
	velocity *= 1 + (position->y_ - y) * 0.1 * mass * timeStep;
	position->y_ = y;
}

void Unit::addAim(Aim* aim) {
	aims.add(aim);
}

void Unit::addAim(const FutureAim& aim, bool append) {
	if (!append) {
		clearAims();
	}
	aims.add(aim);
}

void Unit::drawLineTo(const Urho3D::Vector3& second,
                      const Urho3D::Color& color = Urho3D::Color::WHITE) const {
	drawLine(*position, second + *position, color);
}

void Unit::drawLine(const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                    const Urho3D::Color& color = Urho3D::Color::WHITE) {
	DebugLineRepo::geometry->DefineVertex(first);
	DebugLineRepo::geometry->DefineColor(color);
	DebugLineRepo::geometry->DefineVertex(second);
	DebugLineRepo::geometry->DefineColor(color);
}

void Unit::debug(DebugUnitType type, ForceStats& stats) {
	if constexpr (UNIT_DEBUG_ENABLED) {
		if (billboardBar->enabled_) {
			switch (type) {
			case DebugUnitType::NONE:
				break;
			case DebugUnitType::VELOCITY:
				drawLineTo({velocity.x_, 0.5, velocity.y_});
				break;
			case DebugUnitType::ACCELERATION:
				drawLineTo({acceleration.x_, 0.5, acceleration.y_});
				break;
			case DebugUnitType::SEPARATION_UNITS:
				drawLineTo({stats.sepUnitLast.x_, 0.5, stats.sepUnitLast.y_});
				break;
			case DebugUnitType::SEPARATION_OBSTACLE:
				drawLineTo({stats.sepObstLast.x_, 0.5, stats.sepObstLast.y_});
				break;
			case DebugUnitType::DESTINATION:
				drawLineTo({stats.destLast.x_, 0.5, stats.destLast.y_});
				break;
			case DebugUnitType::FORMATION:
				drawLineTo({stats.formLast.x_, 0.5, stats.formLast.y_});
				break;
			case DebugUnitType::ESCAPE:
				drawLineTo({stats.escaLast.x_, 0.5, stats.escaLast.y_});
				break;
			case DebugUnitType::ALL_FORCE:
				drawLineTo({velocity.x_, 0.5, velocity.y_});
				drawLineTo({stats.sepUnitLast.x_, 0.5, stats.sepUnitLast.y_}, Urho3D::Color::RED);
				drawLineTo({stats.sepObstLast.x_, 0.5, stats.sepObstLast.y_}, Urho3D::Color::GREEN);
				drawLineTo({stats.destLast.x_, 0.5, stats.destLast.y_}, Urho3D::Color::BLUE);
				drawLineTo({stats.formLast.x_, 0.5, stats.formLast.y_}, Urho3D::Color::YELLOW);
				drawLineTo({stats.escaLast.x_, 0.5, stats.escaLast.y_}, Urho3D::Color::CYAN);
				break;
			case DebugUnitType::AIM:
				if (aims.hasCurrent()) {
					auto lines = aims.getDebugLines(this);
					for (int i = 0; i < lines.size() - 1; ++i) {
						drawLine(lines[i], lines[i + 1]);
					}
				}
				break;
			case DebugUnitType::INTERACT:
				for (auto toInteract : thingsToInteract) {
					drawLine(*position, *toInteract->getPosition());
				}
				break;
			default: ;
			}
		}
	}
}

void Unit::clearAims() {
	aims.clear();
}

void Unit::removeCurrentAim() {
	aims.removeCurrentAim();
}

void Unit::setIndexToInteract(int index) {
	indexToInteract = index;
}

Urho3D::String& Unit::toMultiLineString() {
	menuString = dbUnit->name + " " + dbLevel->name;
	menuString.Append("\nAtak: ").Append(Urho3D::String(attackCoef))
	          .Append("\nObrona: ").Append(Urho3D::String(defenseCoef))
	          .Append("\nZdrowie: ").Append(Urho3D::String(hpCoef))
	          .Append("/").Append(Urho3D::String(maxHpCoef));
	return menuString;
}

void Unit::action(char id, const ActionParameter& parameter) {
	switch (id) {
	case UnitOrder::GO:
		StateManager::changeState(this, UnitState::GO_TO, parameter);
		break;
	case UnitOrder::STOP:
		StateManager::changeState(this, UnitState::STOP);
		break;
	case UnitOrder::CHARGE:
		StateManager::changeState(this, UnitState::CHARGE, parameter);
		break;
	case UnitOrder::ATTACK: break;
	case UnitOrder::DEAD:
		StateManager::changeState(this, UnitState::DEAD);
		break;
	case UnitOrder::DEFEND:
		StateManager::changeState(this, UnitState::DEFEND);
		break;
	case UnitOrder::FOLLOW:
		StateManager::changeState(this, UnitState::FOLLOW, parameter);
		break;
	default: ;
	}
}

std::string Unit::getValues(int precision) {
	const int position_x = position->x_ * precision;
	const int position_z = position->z_ * precision;
	const int state = static_cast<int>(this->state);
	const int velocity_x = velocity.x_ * precision;
	const int velocity_z = velocity.y_ * precision;
	return Physical::getValues(precision)
		+ std::to_string(position_x) + "," +
		std::to_string(position_z) + "," +
		std::to_string(state) + "," +
		std::to_string(velocity_x) + "," +
		std::to_string(velocity_z) + "," +
		std::to_string(-1);

}

void Unit::addUpgrade(db_unit_upgrade* upgrade) {
	for (auto& i : upgrades) {
		if (i->path == upgrade->path) {
			i = upgrade;
			return;
		}
	}
	upgrades.push_back(upgrade);
}

void Unit::changeColor(float value, float maxValue) const {
	changeMaterial(Game::getColorPeletteRepo()->getColor(ColorPallet::RED, value, maxValue), model);
}

void Unit::changeColor(ColorMode mode) {
	switch (mode) {
	case ColorMode::BASIC:
		return changeMaterial(basic, model);
	case ColorMode::VELOCITY:
		return changeColor(velocity.LengthSquared(), maxSpeed * maxSpeed);
	case ColorMode::STATE:
		return changeMaterial(Game::getColorPeletteRepo()->getColor(state), model);
	case ColorMode::FORMATION:
		if (formation != -1) {
			changeColor(Game::getFormationManager()->getPriority(this), 3.0f);
		}
	}
}

void Unit::clean() {
	Physical::clean();
}

void Unit::setState(UnitState _state) {
	state = _state;
}

bool Unit::hasResource() {
	return isFirstThingAlive();
}

void Unit::load(dbload_unit* unit) {
	Physical::load(unit);
	state = UnitState(unit->state); //TODO nie wiem czy nie przepisaæpoprzez przejscie?
	velocity = {unit->vel_x, unit->vel_z};
}

int Unit::getLevel() {
	return dbLevel->level;
}

float Unit::getMaxHpBarSize() {
	return 0.4f;
}

void Unit::setFormation(short _formation) {
	formation = _formation;
}

void Unit::resetFormation() {
	formation = -1;
	posInFormation = -1;
}

void Unit::setPositionInFormation(short _pos) {
	posInFormation = _pos;
}

std::string Unit::getColumns() {
	return Physical::getColumns() +
		"position_x		INT     NOT NULL,"
		"position_z		INT     NOT NULL,"
		"state			INT     NOT NULL,"
		"velocity_x		INT     NOT NULL,"
		"velocity_z		INT     NOT NULL";
}

void Unit::applyForce(double timeStep) {
	if (state == UnitState::ATTACK) {
		return;
	}

	velocity *= 0.5f; //TODO to dac jaki wspolczynnik tarcia terenu
	velocity += acceleration * (timeStep / mass);
	const float velLength = velocity.LengthSquared();
	if (velLength < minSpeed * minSpeed) {
		if (state == UnitState::MOVE) {
			StateManager::changeState(this, UnitState::STOP);
		}
	} else {
		if (velLength > maxSpeed * maxSpeed) {
			velocity.Normalize();
			velocity *= maxSpeed;
		}
		if (state == UnitState::STOP) {
			StateManager::changeState(this, UnitState::MOVE);
		}
		if (rotatable && velLength > 2 * minSpeed * minSpeed) {
			node->SetDirection(Urho3D::Vector3(velocity.x_, 0, velocity.y_));
		}
	}
}

int Unit::getDbID() {
	return dbUnit->id;
}

bool Unit::bucketHasChanged(int _bucketIndex, char param) const {
	return teamBucketIndex[param] != _bucketIndex;
}

void Unit::setBucket(int _bucketIndex, char param) {
	teamBucketIndex[param] = _bucketIndex;
}

Urho3D::Vector2 Unit::getPosToUse(Unit* follower) const {
	for (auto useSocket : useSockets) {
		if (useSocket) {
			float dist = minimalDistance + follower->getMinimalDistance();
		}
	}
	return {};
}

std::tuple<Urho3D::Vector2, float, int> Unit::closest(Physical* toFollow, 
                                                          const std::function<
	                                                          std::tuple<Urho3D::Vector2, int>(
		                                                          Physical*, Unit*)>&
                                                          positionFunc) {
	auto [pos, indexOfPos] = positionFunc(toFollow, this);
	const float distance = sqDist(pos, *this->getPosition());
	return std::tuple<Urho3D::Vector2, float, int>(pos, distance, indexOfPos);
}

std::tuple<Physical*, float, int> Unit::closestPhysical(std::vector<Physical*>* things,
                                                            const std::function<bool(Physical*)>& condition,
                                                            const std::function<
	                                                            std::tuple<Urho3D::Vector2, int>(
		                                                            Physical*, Unit*)>&
                                                            positionFunc) {
	float minDistance = 99999;
	Physical* closestPhy = nullptr;
	int bestIndex = -1;
	for (auto entity : *things) {
		if (entity->isAlive() && condition(entity)) {
			auto [pos, distance, indexOfPos] = closest(entity, positionFunc);

			if (distance <= minDistance) {
				minDistance = distance;
				closestPhy = entity;
				bestIndex = indexOfPos;
			}
		}
	}
	return std::tuple<Physical*, float, int>(closestPhy, minDistance, bestIndex);
}
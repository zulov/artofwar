#include "Unit.h"
#include <algorithm>
#include <string>
#include "Game.h"
#include "../ValueType.h"
#include "colors/ColorPaletteRepo.h"
#include "database/DatabaseCache.h"
#include "debug/DebugLineRepo.h"
#include "debug/DebugUnitType.h"
#include "objects/ObjectEnums.h"
#include "objects/unit/ChargeData.h"
#include "objects/unit/MissileData.h"
#include "objects/unit/SimColorMode.h"
#include "order/IndividualOrder.h"
#include "order/enums/UnitAction.h"
#include "scene/load/dbload_container.h"
#include "simulation/env/Environment.h"
#include "simulation/force/ForceStats.h"
#include "simulation/formation/FormationManager.h"
#include "state/StateManager.h"
#include "utils/consts.h"


Unit::Unit(Urho3D::Vector3& _position, int id, int player, int level) : Physical(_position),
                                                                        state(UnitState::STOP) {
	dbUnit = Game::getDatabase()->getUnit(id);
	dbLevel = dbUnit->getLevel(level).value(); //TODO bug value
	setPlayerAndTeam(player);
	loadXml("Objects/units/" + dbLevel->nodeName);

	basic = model->GetMaterial(0);
	if (StateManager::validateState(dbUnit->id, UnitState::CHARGE)) {
		chargeData = new ChargeData(150, 2);
	}

	if (StateManager::validateState(dbUnit->id, UnitState::SHOT)) {
		missileData = new MissileData(150, 2);
	}


	for (auto& bucket : teamBucketIndex) {
		bucket = INT_MIN;
	}
	std::fill_n(useSockets, USE_SOCKETS_NUMBER, false);
}

Unit::~Unit() {
	delete chargeData;
	delete missileData;
}

bool Unit::isAlive() const {
	return state != UnitState::DEAD && state != UnitState::DISPOSE;
}

void Unit::populate() {
	maxSeparationDistance = dbLevel->maxSep;
	maxSpeed = dbLevel->maxSpeed;
	minSpeed = dbLevel->minSpeed;
	minimalDistance = dbLevel->minDist;
	attackInterest = dbLevel->attackRange * 10;
	rotatable = dbUnit->rotatable;
	actionState = UnitState(dbUnit->actionState);

	collectSpeed = dbLevel->collectSpeed;

	hp = dbLevel->maxHp;
	maxHp = dbLevel->maxHp;
	attackCoef = dbLevel->attack;
	defenseCoef = dbLevel->defense;
	attackSpeed = dbLevel->attackSpeed / 100.f;
}

void Unit::checkAim() {
	if (aims.ifReach(this)) {
		StateManager::changeState(this, UnitState::MOVE);
	}
}

void Unit::move(double timeStep) {
	if (state != UnitState::STOP) {
		position.x_ += velocity.x_ * timeStep;
		position.z_ += velocity.y_ * timeStep;
		node->SetPosition(position);
		//node->Translate((*velocity) * timeStep, TS_WORLD);
	}
	if (missileData && missileData->isUp()) {
		missileData->update(timeStep, attackCoef);
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
	force *= dbLevel->mass;
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
	hp -= attackCoef * (1 - defenseCoef);

	updateHealthBar();

	if (hp < 0) {
		StateManager::changeState(this, UnitState::DEAD);
	}
}

void Unit::actionIfCloseEnough(UnitAction order, Physical* closest, int indexToInteract,
                               float sqDistance, float closeRange, float interestRange) {
	if (closest) {
		if (sqDistance < closeRange * closeRange) {
			addOrder(new IndividualOrder(this, order, closest));
		} else if (sqDistance < interestRange * interestRange) {
			addOrder(new IndividualOrder(this, UnitAction::FOLLOW, nullptr, closest));
			addOrder(new IndividualOrder(this, order, closest, true));
		}
	}
}

void Unit::toAction(Physical* closest, float minDistance, int indexToInteract, UnitAction order) {
	actionIfCloseEnough(order, closest, indexToInteract, minDistance, dbLevel->attackRange, attackInterest);
}

void Unit::toAction(Physical* closest, float minDistance, int indexToInteract, UnitAction order,
                    float attackInterest) {
	actionIfCloseEnough(order, closest, indexToInteract, minDistance, dbLevel->attackRange, attackInterest);
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
	velocity *= 1 + (position.y_ - y) * 0.1 * dbLevel->mass * timeStep;
	position.y_ = y;
}

void Unit::addOrder(UnitOrder* aim) {
	aims.add(aim);
}

void Unit::setAim(Aim* aim) {
	aims.set(aim);
}

void Unit::drawLineTo(const Urho3D::Vector3& second,
                      const Urho3D::Color& color = Urho3D::Color::WHITE) const {
	DebugLineRepo::drawLine(DebugLineType::UNIT_LINES, position, second + position, color);
}

void Unit::debug(DebugUnitType type, ForceStats& stats) {
	if constexpr (DEBUG_LINES_ENABLED) {
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
					if (!lines.empty()) {
						for (int i = 0; i < lines.size() - 1; ++i) {
							DebugLineRepo::drawLine(DebugLineType::UNIT_LINES, lines[i], lines[i + 1]);
						}
					}
				}
				break;
			case DebugUnitType::INTERACT:
				for (auto toInteract : thingsToInteract) {
					DebugLineRepo::drawLine(DebugLineType::UNIT_LINES, position, toInteract->getPosition());
				}
				break;
			default: ;
			}
		}
	}
}

void Unit::removeCurrentAim() {
	aims.removeCurrentAim();
}

void Unit::setIndexToInteract(int index) {
	indexToInteract = index;
}

Urho3D::String Unit::toMultiLineString() {
	return Urho3D::String(dbUnit->name + " " + dbLevel->name)
	       .Append("\nAtak: ").Append(Urho3D::String(attackCoef))
	       .Append("\nObrona: ").Append(Urho3D::String(defenseCoef))
	       .Append("\nZdrowie: ").Append(Urho3D::String(hp))
	       .Append("/").Append(Urho3D::String(maxHp))
	       .Append("\nStan:").Append(Urho3D::String(magic_enum::enum_name(state).data()));

}

void Unit::action(UnitAction unitAction) {
	action(unitAction, Consts::EMPTY_ACTION_PARAMETER);
}

bool Unit::isFirstThingInSameSocket() const {
	return !indexHasChanged;
}

void Unit::action(UnitAction unitAction, const ActionParameter& parameter) {
	switch (unitAction) {
	case UnitAction::GO:
		StateManager::changeState(this, UnitState::GO_TO, parameter);
		break;
	case UnitAction::STOP:
		StateManager::changeState(this, UnitState::STOP, parameter);
		break;
	case UnitAction::CHARGE:
		StateManager::changeState(this, UnitState::CHARGE, parameter);
		break;
	case UnitAction::ATTACK:
		StateManager::changeState(this, UnitState::ATTACK, parameter);
		break;
	case UnitAction::DEAD:
		StateManager::changeState(this, UnitState::DEAD, parameter);
		break;
	case UnitAction::DEFEND:
		resetFormation();
		StateManager::changeState(this, UnitState::DEFEND, parameter);
		break;
	case UnitAction::FOLLOW:
		StateManager::changeState(this, UnitState::FOLLOW, parameter);
		break;
	case UnitAction::COLLECT:
		StateManager::changeState(this, UnitState::COLLECT, parameter);
		break;
	default: ;
	}
}

void Unit::changeColor(float value, float maxValue) const {
	changeMaterial(Game::getColorPaletteRepo()->getColor(ColorPallet::RED, value, maxValue), model);
}

void Unit::changeColor(SimColorMode mode) {
	switch (mode) {
	case SimColorMode::BASIC:
		return changeMaterial(basic, model);
	case SimColorMode::VELOCITY:
		return changeColor(velocity.LengthSquared(), maxSpeed * maxSpeed);
	case SimColorMode::STATE:
		return changeMaterial(Game::getColorPaletteRepo()->getColor(state), model);
	case SimColorMode::FORMATION:
		if (formation != -1) {
			changeColor(Game::getFormationManager()->getPriority(this), 3.0f);
		}
	}
}

void Unit::setState(UnitState _state) {
	state = _state;
}

void Unit::load(dbload_unit* unit) {
	Physical::load(unit);
	state = UnitState(unit->state); //TODO nie wiem czy nie przepisaæpoprzez przejscie?
	velocity = {unit->vel_x, unit->vel_z};
}

ObjectType Unit::getType() const {
	return ObjectType::UNIT;
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

void Unit::clearAims() {
	aims.clear();
}

bool Unit::closeEnoughToAttack() {
	return dirTo(position, getPosToUse()).LengthSquared() < dbLevel->attackRange * dbLevel->attackRange;
}

bool Unit::isInRightSocket() const {
	return indexToInteract == getMainBucketIndex();
}

std::string Unit::getColumns() {
	return Physical::getColumns() +
		"position_x		INT     NOT NULL,"
		"position_z		INT     NOT NULL,"
		"state			INT     NOT NULL,"
		"velocity_x		INT     NOT NULL,"
		"velocity_z		INT     NOT NULL";
}

std::string Unit::getValues(int precision) {
	const int position_x = position.x_ * precision;
	const int position_z = position.z_ * precision;
	const int state = static_cast<int>(this->state);
	const int velocity_x = velocity.x_ * precision;
	const int velocity_z = velocity.y_ * precision;
	return Physical::getValues(precision)
		+ std::to_string(position_x) + "," +
		std::to_string(position_z) + "," +
		std::to_string(state) + "," +
		std::to_string(velocity_x) + "," +
		std::to_string(velocity_z);

}

void Unit::applyForce(double timeStep) {
	velocity *= 0.5f; //TODO to dac jaki wspolczynnik tarcia terenu
	velocity += acceleration * (timeStep / dbLevel->mass);
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

short Unit::getId() {
	return dbUnit->id;
}

float Unit::getAttackRange() const { return dbLevel->attackRange; }

void Unit::setBucket(int _bucketIndex) {
	Physical::setBucket(_bucketIndex);
}

bool Unit::bucketHasChanged(int _bucketIndex, char param) const {
	return teamBucketIndex[param] != _bucketIndex;
}

void Unit::setBucket(int _bucketIndex, char param) {
	teamBucketIndex[param] = _bucketIndex;
}

bool Unit::isSlotOccupied(int indexToInteract) {
	const int index = Game::getEnvironment()->getRevertCloseIndex(getMainCell(), indexToInteract);

	return useSockets[index];
}

std::tuple<Physical*, float, int> Unit::closestPhysical(std::vector<Physical*>* things,
                                                        const std::function<bool(Physical*)>& condition) {
	float minDistance = 99999;
	Physical* closestPhy = nullptr;
	int bestIndex = -1;
	for (auto entity : *things) {
		if (entity->isAlive() && condition(entity)) {
			auto opt = entity->getPosToUseWithIndex(this);
			if (opt.has_value()) {
				auto [pos, distance, indexOfPos] = opt.value();
				if (distance <= minDistance) {
					minDistance = distance;
					closestPhy = entity;
					bestIndex = indexOfPos;
				}
			}
		}
	}
	return {closestPhy, minDistance, bestIndex};
}

bool Unit::isFirstThingAlive() const {
	return !thingsToInteract.empty()
		&& thingsToInteract[0] != nullptr
		&& thingsToInteract[0]->isUsable();
}

void Unit::clean() {
	Physical::clean();
	thingsToInteract.erase(std::remove_if(
		                       thingsToInteract.begin(), thingsToInteract.end(),
		                       [](Physical* physical) {
			                       return physical == nullptr || !physical->isAlive();
		                       }),
	                       thingsToInteract.end());
}

float Unit::getValueOf(ValueType type) const {
	return dbLevel->aiProps->getValueOf(type) * (hp / maxHp);
}

void Unit::fillValues(std::span<float> weights) const {
	std::copy(dbLevel->aiProps->params, dbLevel->aiProps->params + 3, weights.begin());
	auto percent = (hp / maxHp);
	for (auto& weight : weights) {
		weight *= percent;
	}
}

Urho3D::Vector2 Unit::getSocketPos(Unit* toFollow, int i) const {
	const auto vector = Consts::circleCords[i] * (minimalDistance + toFollow->getMinimalDistance()) * 2;
	return {toFollow->getPosition().x_ + vector.x_, toFollow->getPosition().z_ + vector.y_};
}

std::optional<std::tuple<Urho3D::Vector2, float, int>> Unit::getPosToUseWithIndex(Unit* follower) {
	float minDistance = 99999;
	Urho3D::Vector2 closest;
	int closestIndex = -1;
	const int mainIndex = getMainBucketIndex();
	const std::vector<char>& closeTabIndexes = Game::getEnvironment()->getCloseTabIndexes(mainIndex);
	const std::vector<short>& closeIndexes = Game::getEnvironment()->getCloseIndexs(mainIndex);
	for (auto i : closeTabIndexes) {
		if (!useSockets[i]) {
			int index = mainIndex + closeIndexes[i];
			if (Game::getEnvironment()->cellInState(index, {CellState::EMPTY, CellState::COLLECT, CellState::ATTACK})) {
				Urho3D::Vector2 posToFollow = getSocketPos(this, i);
				if (index == follower->getMainCell()) {
					return {{posToFollow, 0, index}};
				}
				setClosest(minDistance, closest, closestIndex, index, posToFollow, follower->getPosition());
			}
		}
	}
	if (closestIndex >= 0) {
		return {{closest, minDistance, closestIndex}};
	}
	return {};
}

Urho3D::Vector2 Unit::getPosToUse() {
	if (isFirstThingAlive() && indexToInteract != -1) {
		return getSocketPos(dynamic_cast<Unit*>(thingsToInteract[0]), indexToInteract);
	}
	return {position.x_, position.z_};
}

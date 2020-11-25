#include "Unit.h"
#include <algorithm>
#include <string>
#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "colors/ColorPallet.h"
#include "database/DatabaseCache.h"
#include "debug/DebugLineRepo.h"
#include "debug/DebugUnitType.h"
#include "math/VectorUtils.h"
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
	if (StateManager::validateState(dbUnit->id, UnitState::CHARGE) && dbLevel->canChargeAttack) {
		chargeData = new ChargeData(150, 2);
	}

	if (StateManager::validateState(dbUnit->id, UnitState::SHOT) && dbLevel->canCollect) {
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
	maxSpeed = dbLevel->maxSpeed;
	hp = dbLevel->maxHp;
}

void Unit::checkAim() {
	if (aims.ifReach(this)) {
		StateManager::changeState(this, UnitState::MOVE);
	}
}

void Unit::updatePosition() {
	if (isVisible) {
		if (velocity.LengthSquared() > 4 * dbLevel->sqMinSpeed) {
			node->SetTransform(
				position,
				Urho3D::Quaternion(Urho3D::Vector3::FORWARD, Urho3D::Vector3(velocity.x_, 0, velocity.y_)));
		} else {
			node->SetPosition(position);
		}
	}
}

void Unit::move(float timeStep, const Urho3D::Vector4& boundary) {
	const bool prevVisible = isVisible;
	isVisible = ifVisible(boundary);
	if (state != UnitState::STOP) {
		position.x_ += velocity.x_ * timeStep;
		position.z_ += velocity.y_ * timeStep;
		updatePosition();
	} else if (prevVisible == false && isVisible == true) {
		node->SetPosition(position);
	}
	if (missileData && missileData->isUp()) {
		missileData->update(timeStep, dbLevel->rangeAttackVal);
	}
}

bool Unit::ifVisible(const Urho3D::Vector4& boundary) const {
	return (boundary.x_ < position.x_ || boundary.x_ < node->GetPosition().x_)
		&& (position.x_ < boundary.y_ || node->GetPosition().x_ < boundary.y_)
		&& (boundary.z_ < position.z_ || boundary.z_ < node->GetPosition().z_)
		&& (position.z_ < boundary.w_ || node->GetPosition().z_ < boundary.w_);

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
	force *= dbLevel->mass * aimCoef * 2.f;
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

float Unit::absorbAttack(float attackCoef) {
	auto val = attackCoef * (1 - dbLevel->armor);
	hp -= val;

	updateHealthBar();

	if (hp < 0) {
		StateManager::changeState(this, UnitState::DEAD);
	}
	return val;
}

void Unit::actionIfCloseEnough(UnitAction order, Physical* closest, float sqDistance, bool force) {
	if (closest && (force || sqDistance < dbLevel->sqAttackInterest)) {
		addOrder(new IndividualOrder(this, order, closest, true));
	}
}

void Unit::toAction(Physical* closest, float minDistance, UnitAction order, bool force) {
	actionIfCloseEnough(order, closest, minDistance, force);
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
	velocity *= 1 + (position.y_ - y) * 0.1f * dbLevel->mass * timeStep;
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
		if (selected && isVisible) {
			switch (type) {
			case DebugUnitType::NONE:
				break;
			case DebugUnitType::VELOCITY:
				drawLineTo({velocity.x_, 0.5f, velocity.y_});
				break;
			case DebugUnitType::ACCELERATION:
				drawLineTo({acceleration.x_, 0.5f, acceleration.y_});
				break;
			case DebugUnitType::SEPARATION_UNITS:
				drawLineTo({stats.sepUnitLast.x_, 0.5f, stats.sepUnitLast.y_});
				break;
			case DebugUnitType::SEPARATION_OBSTACLE:
				drawLineTo({stats.sepObstLast.x_, 0.5f, stats.sepObstLast.y_});
				break;
			case DebugUnitType::DESTINATION:
				drawLineTo({stats.destLast.x_, 0.5f, stats.destLast.y_});
				break;
			case DebugUnitType::FORMATION:
				drawLineTo({stats.formLast.x_, 0.5f, stats.formLast.y_});
				break;
			case DebugUnitType::ESCAPE:
				drawLineTo({stats.escaLast.x_, 0.5f, stats.escaLast.y_});
				break;
			case DebugUnitType::ALL_FORCE:
				drawLineTo({velocity.x_, 0.5f, velocity.y_});
				drawLineTo({stats.sepUnitLast.x_, 0.5f, stats.sepUnitLast.y_}, Urho3D::Color::RED);
				drawLineTo({stats.sepObstLast.x_, 0.5f, stats.sepObstLast.y_}, Urho3D::Color::GREEN);
				drawLineTo({stats.destLast.x_, 0.5f, stats.destLast.y_}, Urho3D::Color::BLUE);
				drawLineTo({stats.formLast.x_, 0.5f, stats.formLast.y_}, Urho3D::Color::YELLOW);
				drawLineTo({stats.escaLast.x_, 0.5f, stats.escaLast.y_}, Urho3D::Color::CYAN);
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
	       .Append("\nAtak: ").Append(Urho3D::String(dbLevel->closeAttackVal))
	       .Append(Urho3D::String(dbLevel->rangeAttackVal))
	       .Append(Urho3D::String(dbLevel->chargeAttackVal))
	       .Append(Urho3D::String(dbLevel->buildingAttackVal))
	       .Append("\nObrona: ").Append(Urho3D::String(dbLevel->armor))
	       .Append("\nZdrowie: ").Append(Urho3D::String(hp))
	       .Append("/").Append(Urho3D::String(dbLevel->maxHp))
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

int Unit::getLevelNum() {
	return dbLevel->level;
}

float Unit::getMaxHpBarSize() const {
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
	float dist = sqDist(position, getPosToUse());
	float sqRange;
	return dbLevel->canCloseAttack && sqRange < dbLevel->sqCloseAttackRange
		|| dbLevel->canRangeAttack && sqRange < dbLevel->sqRangeAttackRange;
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

void Unit::applyForce(float timeStep) {
	velocity *= 0.5f; //TODO to dac jaki wspolczynnik tarcia terenu
	velocity += acceleration * (timeStep / dbLevel->mass);
	const float velLength = velocity.LengthSquared();
	if (velLength < dbLevel->sqMinSpeed) {
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
	}
}

short Unit::getId() {
	return dbUnit->id;
}

float Unit::getAttackRange() const {
	if (dbLevel->canRangeAttack) {
		return dbLevel->rangeAttackRange;
	}

	if (dbLevel->canRangeAttack) {
		return dbLevel->closeAttackRange;
	}
	assert(false);
	return 2.f;
}

float Unit::getMinimalDistance() const {
	return dbLevel->minDist;
}

float Unit::getMaxSeparationDistance() const {
	return dbLevel->maxSep;
}


UnitState Unit::getActionState() const {
	return UnitState(dbUnit->actionState);
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
	thingsToInteract.erase(std::remove_if(thingsToInteract.begin(), thingsToInteract.end(), isAlivePred),
	                       thingsToInteract.end());
}

void Unit::fillValues(std::span<float> weights) const {
	assert(weights.size(), dbLevel->dbUnitMetric->getParamsNormAsSpan().size());
	auto data = dbLevel->dbUnitMetric->getParamsAsSpan();

	std::copy(data.begin(), data.end(), weights.begin());
	auto percent = hp / dbLevel->maxHp;
	for (auto& weight : weights) {
		weight *= percent;
	}
}

void Unit::addValues(std::span<float> vals) const {
	const auto percent = hp / dbLevel->maxHp;
	assert(vals.size()==dbLevel->dbUnitMetric->getParamsAsSpan().size()-1); //without cost
	for (int i = 0; i < vals.size(); ++i) {
		vals[i] += percent * dbLevel->dbUnitMetric->getParamsAsSpan()[i];
	}
}

unsigned short Unit::getMaxHp() const {
	return dbLevel->maxHp;
}

Urho3D::Vector2 Unit::getSocketPos(Unit* toFollow, int i) const {
	const auto vector = Consts::circleCords[i] * (dbLevel->minDist + toFollow->getMinimalDistance()) * 2;
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
			if (Game::getEnvironment()->cellIsPassable(index)) {
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

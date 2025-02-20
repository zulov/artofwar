﻿#include "Unit.h"

#include <Urho3D/Resource/Localization.h>

#include "Game.h"
#include "colors/ColorPaletteRepo.h"
#include "colors/ColorPallet.h"
#include "database/DatabaseCache.h"
#include "database/db_strcut.h"
#include "debug/DebugLineRepo.h"
#include "debug/DebugUnitType.h"
#include "math/VectorUtils.h"
#include "objects/unit/ChargeData.h"
#include "objects/unit/SimColorMode.h"
#include "order/IndividualOrder.h"
#include "order/enums/UnitAction.h"
#include "scene/load/dbload_container.h"
#include "env/Environment.h"
#include "simulation/force/ForceStats.h"
#include "simulation/formation/FormationManager.h"
#include "state/StateManager.h"
#include "camera/CameraInfo.h"
#include "math/MathUtils.h"
#include "utils/consts.h"
#include "utils/Flags.h"
#include "objects/NodeUtils.h"

Unit::Unit(Urho3D::Vector3& _position, short dbId, char playerId, char teamId, int level, UId uId) : Physical(_position, uId),
                                                                        state(UnitState::STOP),
                                                                        nextState(UnitState::STOP) {
	dbUnit = Game::getDatabase()->getUnit(dbId);
	dbLevel = dbUnit->getLevel(level).value(); //TODO bug value
	setPlayerAndTeam(playerId, teamId);
	loadXml("Objects/units/" + dbLevel->node);
	populate();

	if (dbUnit->typeCalvary) {
		chargeData = new ChargeData(150, 2);
	}
	shouldUpdate = true;
}

Unit::~Unit() {
	delete chargeData;
}

bool Unit::isAlive() const {
	return state != UnitState::DEAD && state != UnitState::DISPOSE;
}

void Unit::populate() {
	maxSpeed = dbLevel->maxSpeed;
	invMaxHp = dbLevel->invMaxHp;
	hp = dbLevel->maxHp;
	dbId = dbUnit->id;
}

void Unit::checkAim() {
	if (aims.process(this)) {
		StateManager::changeState(this, UnitState::MOVE);
	}
}

void Unit::updatePosition() const {
	if (shouldUpdate) {
		if (isInStates(state, {UnitState::ATTACK, UnitState::COLLECT, UnitState::SHOT}) && isFirstThingAlive()) {
			setTransform(dirTo(position, thingToInteract->getPosition()));
		} else if (velocity.LengthSquared() > 4 * dbLevel->sqMinSpeed) {
			setTransform(velocity);
		} else {
			node->SetPosition(position);
		}
	}
}

bool Unit::move(float timeStep, const CameraInfo* camInfo) {
	bool hasMoved = false;
	const bool prevVisible = shouldUpdate;

	if (state != UnitState::STOP && velocity != Urho3D::Vector2::ZERO) {
		hasMoved = true;
		position.x_ += velocity.x_ * timeStep;
		position.z_ += velocity.y_ * timeStep;
		shouldUpdate = ifVisible(true, camInfo);
		updatePosition();
	} else {
		shouldUpdate = ifVisible(false, camInfo);
	}
	if (prevVisible == false && shouldUpdate == true) {
		node->SetPosition(position);
	}

	return hasMoved;
}

bool Unit::ifVisible(bool hasMoved, const CameraInfo* camInfo) const {
	if (SIM_GLOBALS.HEADLESS) {
		return false;
	}
	if (!(camInfo->hasMoved || hasMoved)) {
		return shouldUpdate;
	}
	auto& boundary = camInfo->boundary;

	return (boundary.x_ < position.x_ || boundary.x_ < node->GetPosition().x_)
		&& (position.x_ < boundary.y_ || node->GetPosition().x_ < boundary.y_)
		&& (boundary.z_ < position.z_ || boundary.z_ < node->GetPosition().z_)
		&& (position.z_ < boundary.w_ || node->GetPosition().z_ < boundary.w_);
}

void Unit::setAcceleration(Urho3D::Vector2& _acceleration) {
	acceleration = _acceleration;
	assert(!_acceleration.IsNaN());
	limitTo(acceleration, dbLevel->maxForce);
}

void Unit::forceGo(float boostCoef, float aimCoef, Urho3D::Vector2& force) const {
	scaleTo(force, boostCoef);
	force -= velocity;
	force *= dbLevel->mass * aimCoef * 2.f;
}

Urho3D::Vector2 Unit::getDestination(float boostCoef, float aimCoef) {
	aims.clearExpired();
	if (aims.hasCurrent()) {
		auto force = aims.getDirection(this);
		forceGo(boostCoef, aimCoef, force);
		return force;
	}
	return {};
}

std::pair<float, bool> Unit::absorbAttack(float attackCoef) {
	if (hp <= 0.f) {
		return {0.f, false};
	}
	auto val = attackCoef * (1 - dbLevel->armor);
	hp -= val;

	if (hp <= 0) {
		StateManager::changeState(this, UnitState::DEAD);
		return {val, true};
	}
	return {val, false};
}

bool Unit::toActionIfInRange(Physical* closest, UnitAction order) {
	if (closest && sqDistAs2D(getPosition(), closest->getPosition()) < dbLevel->sqSightRadius) {
		return toAction(closest, order);
	}
	return false;
}

bool Unit::toAction(Physical* closest, UnitAction order) {
	if (closest) {
		addOrder(new IndividualOrder(this, order, closest, true));
		return true;
	}
	return false;
}

void Unit::updateHeight(float y, float timeStep) {
	velocity *= 1.f + (position.y_ - y) * 0.1f * dbLevel->mass * timeStep;
	position.y_ = y;
}

void Unit::addOrder(IndividualOrder* aim) {
	resetFormation();
	aims.add(aim);
}

void Unit::setIndexChanged(bool changed) {
	indexHasChanged = changed;
}

void Unit::resetToInteract() {
	thingToInteract = nullptr;
	indexToInteract = -1;
	slotToInteract = -1;
}

void Unit::setAim(Aim* aim) {
	aims.set(aim);
}

void Unit::setTransform(const Urho3D::Vector2& rotation) const {
	node->SetTransform(position,
	                   Urho3D::Quaternion(Urho3D::Vector3::FORWARD, Urho3D::Vector3(rotation.x_, 0.f, rotation.y_)));
}

void Unit::drawLineTo(const Urho3D::Vector2& second,
                      const Urho3D::Color& color = Urho3D::Color::WHITE) const {
	if (!second.Equals(Urho3D::Vector2::ZERO)) {
		Urho3D::Vector3 to = Urho3D::Vector3(position.x_ + second.x_, position.y_, position.z_ + second.y_);
		DebugLineRepo::drawLine(DebugLineType::UNIT_LINES, position, to, color);
	}
}

void Unit::debug(DebugUnitType type, ForceStats& stats) {
	if constexpr (DEBUG_LINES_ENABLED) {
		if (selected) {
			switch (type) {
			case DebugUnitType::NONE:
				break;
			case DebugUnitType::VELOCITY:
				drawLineTo(velocity);
				break;
			case DebugUnitType::ACCELERATION:
				drawLineTo(acceleration);
				break;
			case DebugUnitType::SEPARATION_UNITS:
				drawLineTo(stats.sepUnitLast);
				break;
			case DebugUnitType::SEPARATION_OBSTACLE:
				drawLineTo(stats.sepObstLast);
				break;
			case DebugUnitType::DESTINATION:
				drawLineTo(stats.destLast);
				break;
			case DebugUnitType::FORMATION:
				drawLineTo(stats.formLast);
				break;
			case DebugUnitType::ESCAPE:
				drawLineTo(stats.escaLast);
				break;
			case DebugUnitType::ALL_FORCE:
				drawLineTo(velocity);
				drawLineTo(stats.sepUnitLast, Urho3D::Color::RED);
				drawLineTo(stats.sepObstLast, Urho3D::Color::GREEN);
				drawLineTo(stats.destLast, Urho3D::Color::BLUE);
				drawLineTo(stats.formLast, Urho3D::Color::YELLOW);
				drawLineTo(stats.escaLast, Urho3D::Color::CYAN);
				break;
			case DebugUnitType::AIM:
				if (aims.hasCurrent()) {
					const auto lines = aims.getDebugLines(this);
					if (!lines.empty()) {
						for (int i = 0; i < lines.size() - 1; ++i) {
							DebugLineRepo::drawLine(DebugLineType::UNIT_LINES, lines[i], lines[i + 1]);
						}
					}
				}
				break;
			case DebugUnitType::INTERACT: //TODO charge
				DebugLineRepo::drawLine(DebugLineType::UNIT_LINES, position, thingToInteract->getPosition());
				break;
			default: ;
			}
			//stats.vectorReset();
		}
	}
}

void Unit::removeCurrentAim() {
	aims.removeCurrentAim();
}

void Unit::setIndexToInteract(int index) {
	indexToInteract = index;
}

Urho3D::String Unit::getInfo() const {
	const auto l10n = Game::getLocalization();
	return Urho3D::String(dbUnit->name + " " + dbLevel->name)
		.AppendWithFormat(l10n->Get("info_unit").CString(),
		                  asStringF(dbLevel->attack, 1).c_str(),
		                  asStringF(dbLevel->attackRange, 1).c_str(),
		                  asStringF(dbLevel->attackReload, 1).c_str(),
		                  asStringF(dbLevel->armor).c_str(),
		                  (int)hp, dbLevel->maxHp,
		                  closeUsers, getMaxCloseUsers(),
		                  rangeUsers, getMaxRangeUsers(),
		                  magic_enum::enum_name(state).data());
}

const Urho3D::String& Unit::getName() const {
	return dbUnit->name;
}

bool Unit::action(UnitAction unitAction) {
	assert(unitAction == UnitAction::STOP || unitAction == UnitAction::DEAD || unitAction == UnitAction::DEFEND);
	return action(unitAction, Consts::EMPTY_ACTION_PARAMETER);
}

bool Unit::action(UnitAction unitAction, const ActionParameter& parameter) {
	switch (unitAction) {
	case UnitAction::GO:
		return StateManager::changeState(this, UnitState::GO, parameter);
	case UnitAction::STOP:
		return StateManager::changeState(this, UnitState::STOP, parameter);
	case UnitAction::CHARGE:
		return StateManager::changeState(this, UnitState::CHARGE, parameter);
	case UnitAction::ATTACK:
		if (dbUnit->typeRange) {
			//TODO perf chyba trzeba dodacparamter do actionParameter
			//TODO zawsze strzelac? nawet z bliska
			if (!parameter.thingToInteract->isInCloseRange(getMainGridIndex())) {
				//jezeli nie jest in close range
				return StateManager::changeState(this, UnitState::SHOT, parameter);
			}
		}
		return StateManager::changeState(this, UnitState::ATTACK, parameter);

	case UnitAction::DEAD:
		return StateManager::changeState(this, UnitState::DEAD, parameter);
	case UnitAction::DEFEND:
		return StateManager::changeState(this, UnitState::DEFEND, parameter);
	case UnitAction::FOLLOW:
		return StateManager::changeState(this, UnitState::FOLLOW, parameter);
	case UnitAction::COLLECT:
		return StateManager::changeState(this, UnitState::COLLECT, parameter);
	default: ;
	}
}

void Unit::setSparseIndex(int index) {
	if (index >= 0) {
		sparseIndexInGrid = index;
	}
}

float Unit::getModelHeight() const {
	return dbLevel->modelHeight;
}

void Unit::setModelData(float modelHeight) const {
	dbLevel->modelHeight = modelHeight;
}

Urho3D::Color Unit::getColor(db_player_colors* col) const {
	return col->unitColor;
}

void Unit::setVisibility(VisibilityType type) {
	if (node) {
		switch (type) {
		case VisibilityType::NONE:
		case VisibilityType::SEEN:
			if (node->IsEnabled()) {
				node->SetEnabled(false);
			}
			break;
		case VisibilityType::VISIBLE: {
			if (!node->IsEnabled()) {
				node->SetEnabled(true);
			}
		}
		}
	}
}

void Unit::resetStateChangePending() {
	stateChangePending = false;
}

void Unit::changeColor(SimColorMode mode) {
	switch (mode) {
	case SimColorMode::BASIC:
		setShaderParam(this, "ColorPercent", 2.f);
		break;
	case SimColorMode::VELOCITY:
		setShaderParam(this, "ColorPercent", velocity.LengthSquared() / maxSpeed * maxSpeed);
		break;
	case SimColorMode::STATE:
		setShaderParam(this, "ColorPercent", ((float)state) / magic_enum::enum_count<UnitState>());
		break;
	case SimColorMode::FORMATION:
		if (formation != -1) {
			setShaderParam(this, "ColorPercent", Game::getFormationManager()->getPriority(this) / 3.f);
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

char Unit::getLevelNum() {
	return dbLevel->level;
}

unsigned short Unit::getMaxHpBarSize() const {
	return 0.4f;
}

float Unit::getAttackVal(Physical* aim) {
	if (aim->getType() == ObjectType::BUILDING) {
		return dbLevel->attack * (1.f + dbLevel->bonusBuilding);
	}
	if (aim->getType() == ObjectType::UNIT) {
		return dbLevel->attack * (1.f + ((Unit*)aim)->getDb()->getBonuses(dbLevel));
	}
	return dbLevel->attack;
}

void Unit::setFormation(short _formation) {
	formation = _formation;
}

void Unit::resetFormation() {
	formation = -1;
	posInState = -1;
}

void Unit::setPositionInFormation(short _pos) {
	posInState = _pos;
}

void Unit::clearAims() {
	aims.clear();
}

void Unit::setNextState(UnitState stateTo, const ActionParameter& actionParameter) {
	const bool mayHaveAim = nextState == UnitState::GO || nextState == UnitState::CHARGE || nextState ==
		UnitState::FOLLOW;
	nextActionParameter.reset(actionParameter, mayHaveAim);
	nextState = stateTo;
	stateChangePending = true;
}

void Unit::setNextState(UnitState stateTo) {
	setNextState(stateTo, Consts::EMPTY_ACTION_PARAMETER);
}

ActionParameter& Unit::getNextActionParameter() {
	return nextActionParameter;
}

bool Unit::hasStateChangePending() const {
	return stateChangePending;
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
	velocity += acceleration * (timeStep * dbLevel->invMass);

	const float lengthSq = velocity.LengthSquared();
	if (lengthSq < dbLevel->sqMinSpeed) {
		if (state == UnitState::MOVE) {
			StateManager::changeState(this, UnitState::STOP);
		}
		if (lengthSq < dbLevel->sqMinSpeed / 1000) {
			velocity = Urho3D::Vector2::ZERO;
		}
	} else {
		limitTo(velocity, maxSpeed, lengthSq);

		if (state == UnitState::STOP) {
			StateManager::changeState(this, UnitState::MOVE);
		}
	}
}

float Unit::getAttackRange() const {
	return dbLevel->attackRange;
}

float Unit::getMinimalDistance() const {
	return dbLevel->minDist;
}

float Unit::getMaxSeparationDistance() const {
	return dbLevel->maxSep;
}

UnitState Unit::getActionState() const {
	return dbUnit->actionState;
}

bool Unit::isFirstThingAlive() const {
	return thingToInteract != nullptr
		&& thingToInteract->isUsable();
}

void Unit::clean() {
	if (notAlive(thingToInteract)) {
		thingToInteract = nullptr;
	}
}

float Unit::getSightRadius() const {
	return dbLevel->sightRadius;
}

Urho3D::Vector2 Unit::getSocketPos(Unit* toFollow, int i) const {
	//TODO bug co to za dziwna funkcja
	const auto vector = Consts::circleCords[i] * (dbLevel->minDist + toFollow->getMinimalDistance()) * 2;
	return {toFollow->getPosition().x_ + vector.x_, toFollow->getPosition().z_ + vector.y_};
}

short Unit::getCostSum() const {
	return dbUnit->getSumCost();
}

bool Unit::isInCloseRange(int index) const {
	return Game::getEnvironment()->isInLocalArea(getMainGridIndex(), index);
}

std::optional<std::tuple<Urho3D::Vector2, float>> Unit::getPosToUseWithDist(Unit* user) {
	float minDistance = 99999;
	Urho3D::Vector2 closest;
	int closestIndex = -1;
	const int mainIndex = getMainGridIndex();

	for (auto [i, val] : Game::getEnvironment()->getCloseTabIndexesWithValue(mainIndex)) {
		if (ifSlotFree(i)) {
			//TODO1
			int index = mainIndex + val;
			if (Game::getEnvironment()->cellIsPassable(index)) {
				//TODO2 to chyba sprawdza to samo prawie?
				Urho3D::Vector2 posToFollow = getSocketPos(this, i);
				if (index == user->getMainGridIndex()) {
					return {{posToFollow, 0}};
				}
				setClosest(minDistance, closest, closestIndex, index, posToFollow, user->getPosition());
			}
		}
	}
	if (closestIndex >= 0) {
		return {{closest, minDistance}};
	}
	return {};
}

std::vector<int> Unit::getIndexesForUse() const {
	std::vector<int> indexes;
	if (belowCloseLimit() <= 0) { return indexes; }
	indexes.reserve(8 - std::popcount(useSockets));
	addIndexesForUse(indexes);

	return indexes;
}

void Unit::addIndexesForUse(std::vector<int>& indexes) const {
	const int mainIndex = getMainGridIndex();
	const auto env = Game::getEnvironment();
	for (auto [i, val] : env->getCloseTabIndexesWithValue(mainIndex)) {
		if (ifSlotFree(i)) {
			int index = mainIndex + val;
			if (env->cellIsPassable(index)) {
				indexes.push_back(index);
			}
		}
	}
}

bool Unit::indexCanBeUse(int index) const {
	const int mainIndex = getMainGridIndex();
	const auto env = Game::getEnvironment();
	for (auto [i, val] : env->getCloseTabIndexesWithValue(mainIndex)) {
		const auto idx = mainIndex + val;
		if (idx == index && ifSlotFree(i)) {
			if (env->cellIsPassable(idx)) {
				return true;
			}
		}
	}
	return false;
}

std::vector<int> Unit::getIndexesForRangeUse(Unit* user) const {
	std::vector<int> indexes;
	if (belowRangeLimit() <= 0) { return indexes; }
	const auto env = Game::getEnvironment();
	const std::vector<int> allIndexes = env->getIndexesInRange(
	                                                           getPosition(), user->getLevel()->attackRange);
	const int mainIndex = getMainGridIndex();
	const std::vector<short>& closeIndexes = env->getCloseIndexs(mainIndex);

	for (auto index : allIndexes) {
		if (env->cellIsAttackable(index)
			&& mainIndex != index
			&& std::ranges::find(closeIndexes, index - mainIndex) == closeIndexes.end()) {
			//TODO better jest juz funkcja intab?
			//czy to ok?
			indexes.push_back(index);
		}
	}
	return indexes;
}

void Unit::setOccupiedIndexSlot(char index, bool value) {
	assert(index < 8);
	assert(index >= 0);
	if (value) {
		useSockets |= Flags::bitFlags[index];
	} else {
		useSockets &= ~Flags::bitFlags[index];
	}
}

bool Unit::ifSlotFree(unsigned char index) const {//TODO perf and clean zwrocic tylko wolne
	assert(index < 8);
	return !(useSockets & Flags::bitFlags[index]);
}

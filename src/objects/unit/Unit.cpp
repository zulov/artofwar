#include "Unit.h"
#include "colors/ColorPaletteRepo.h"
#include "colors/ColorPallet.h"
#include "database/DatabaseCache.h"
#include "database/db_strcut.h"
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
#include "camera/CameraInfo.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "utils/consts.h"
#include "utils/Flags.h"


Unit::Unit(Urho3D::Vector3& _position, int id, int player, int level) : Physical(_position, true),
                                                                        state(UnitState::STOP),
                                                                        nextState(UnitState::STOP) {
	dbUnit = Game::getDatabase()->getUnit(id);
	dbLevel = dbUnit->getLevel(level).value(); //TODO bug value
	setPlayerAndTeam(player);
	loadXml("Objects/units/" + dbLevel->nodeName);
	if (!SIM_GLOBALS.HEADLESS) {
		basic = model->GetMaterial(0);
	}

	if (dbLevel->canChargeAttack) {
		chargeData = new ChargeData(150, 2);
	}

	if (dbLevel->canCollect) {
		missileData = new MissileData(150, 2);
	}
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
	invMaxHp = dbLevel->invMaxHp;
	hp = dbLevel->maxHp;
	id = dbUnit->id;
}

void Unit::checkAim() {
	if (aims.process(this)) {
		StateManager::changeState(this, UnitState::MOVE);
	}
}

void Unit::updatePosition() const {
	if (isVisible) {
		if (isInStates(state, {UnitState::ATTACK, UnitState::COLLECT, UnitState::SHOT}) && isFirstThingAlive()) {
			const auto dir = dirTo(position, thingToInteract->getPosition());

			node->SetTransform(
				position,
				Urho3D::Quaternion(Urho3D::Vector3::FORWARD, Urho3D::Vector3(dir.x_, 0.f, dir.y_)));
		} else if (velocity.LengthSquared() > 4 * dbLevel->sqMinSpeed) {
			node->SetTransform(
				position,
				Urho3D::Quaternion(Urho3D::Vector3::FORWARD, Urho3D::Vector3(velocity.x_, 0.f, velocity.y_)));
		} else {
			node->SetPosition(position);
		}
	}
}

bool Unit::move(float timeStep, const CameraInfo* camInfo) {
	if (missileData && missileData->isUp()) {
		auto [value, died] = missileData->update(timeStep, dbLevel->rangeAttackVal);
		Game::getEnvironment()->addAttack(this, value);
		Game::getPlayersMan()->getPlayer(getPlayer())->addKilled(missileData->getAim());
	}
	bool hasMoved = false;
	const bool prevVisible = isVisible;

	if (state != UnitState::STOP) {
		hasMoved = true;
		position.x_ += velocity.x_ * timeStep;
		position.z_ += velocity.y_ * timeStep;
		isVisible = ifVisible(true, camInfo);
		updatePosition();
	} else {
		isVisible = ifVisible(false, camInfo);
	}
	if (prevVisible == false && isVisible == true) {
		node->SetPosition(position);
	}

	return hasMoved;
}

bool Unit::ifVisible(bool hasMoved, const CameraInfo* camInfo) const {
	if (!(camInfo->hasMoved || hasMoved)) {
		return isVisible;
	}
	auto& boundary = camInfo->boundary;

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

std::pair<float, bool> Unit::absorbAttack(float attackCoef) {
	if (hp <= 0.f) {
		return {0.f, false};
	}
	auto val = attackCoef * (1 - dbLevel->armor);
	hp -= val;

	updateHealthBar();

	if (hp <= 0) {
		StateManager::changeState(this, UnitState::DEAD);
		return {val, true};
	}
	return {val, false};
}

bool Unit::toActionIfInRange(Physical* closest, UnitAction order) {
	if (closest && sqDistAs2D(getPosition(), closest->getPosition()) < dbLevel->sqInterestRange) {
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

void Unit::updateHeight(float y, double timeStep) {
	velocity *= 1 + (position.y_ - y) * 0.1f * dbLevel->mass * timeStep;
	position.y_ = y;
}

void Unit::addOrder(IndividualOrder* aim) {
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
		if (selectedObject || isVisible) {
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
	       .Append("|").Append(Urho3D::String(dbLevel->rangeAttackVal))
	       .Append("|").Append(Urho3D::String(dbLevel->chargeAttackVal))
	       .Append("|").Append(Urho3D::String(dbLevel->buildingAttackVal))
	       .Append("\nObrona: ").Append(Urho3D::String(dbLevel->armor))
	       .Append("\nZdrowie: ").Append(Urho3D::String(hp))
	       .Append("/").Append(Urho3D::String(dbLevel->maxHp))
	       .Append("\nStan:").Append(Urho3D::String(magic_enum::enum_name(state).data()));
}

void Unit::action(UnitAction unitAction) {
	action(unitAction, Consts::EMPTY_ACTION_PARAMETER);
}

bool Unit::indexChanged() const {
	return indexHasChanged;
}

void Unit::action(UnitAction unitAction, const ActionParameter& parameter) {
	switch (unitAction) {
	case UnitAction::GO:
		StateManager::changeState(this, UnitState::GO, parameter);
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

void Unit::setSparseIndex(int index) {
	if (index >= 0) {
		sparseIndexInGrid = index;
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
	stateChangePending = false;
}

void Unit::load(dbload_unit* unit) {
	Physical::load(unit);
	state = UnitState(unit->state); //TODO nie wiem czy nie przepisaæpoprzez przejscie?
	velocity = {unit->vel_x, unit->vel_z};
}

char Unit::getLevelNum() {
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

void Unit::setNextState(UnitState stateTo, const ActionParameter& actionParameter) {
	nextState = stateTo;
	nextActionParameter.reset(actionParameter);
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

float Unit::getAttackRange() const {
	if (dbLevel->canRangeAttack) {
		return dbLevel->rangeAttackRange;
	}

	if (dbLevel->canCloseAttack) {
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
	return dbUnit->actionState;
}

bool Unit::isIndexSlotOccupied(int indexToInteract) {
	const unsigned char index = Game::getEnvironment()->getRevertCloseIndex(getMainBucketIndex(), indexToInteract);
	return ifSlotIsOccupied(index);
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

void Unit::fillValues(std::span<float> weights) const {
	assert(weights.size(), dbLevel->dbUnitMetric->getParamsNormAsSpan().size());
	auto data = dbLevel->dbUnitMetric->getParamsAsSpan();
	assert(validateSpan(__LINE__, __FILE__, data));
	std::copy(data.begin(), data.end(), weights.begin());
	auto percent = hp / dbLevel->maxHp;
	for (auto& weight : weights) {
		weight *= percent;
	}
}

void Unit::addValues(std::span<float> vals) const {
	const auto percent = hp * dbLevel->invMaxHp;
	assert(vals.size()==dbLevel->dbUnitMetric->getParamsAsSpan().size()-1); //without cost
	assert(validateSpan(__LINE__, __FILE__, dbLevel->dbUnitMetric->getParamsAsSpan()));

	for (int i = 0; i < vals.size(); ++i) {
		vals[i] += percent * dbLevel->dbUnitMetric->getParamsAsSpan()[i];
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

std::optional<std::tuple<Urho3D::Vector2, float>> Unit::getPosToUseWithDist(Unit* user) {
	float minDistance = 99999;
	Urho3D::Vector2 closest;
	int closestIndex = -1;
	const int mainIndex = getMainBucketIndex();
	const std::vector<unsigned char>& closeTabIndexes = Game::getEnvironment()->getCloseTabIndexes(mainIndex);
	const std::vector<short>& closeIndexes = Game::getEnvironment()->getCloseIndexs(mainIndex);
	for (auto i : closeTabIndexes) {
		if (!ifSlotIsOccupied(i)) {
			//TODO1
			int index = mainIndex + closeIndexes[i];
			if (Game::getEnvironment()->cellIsPassable(index)) {
				//TODO2 to chyba sprawdza to samo prawie?
				Urho3D::Vector2 posToFollow = getSocketPos(this, i);
				if (index == user->getMainBucketIndex()) {
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

std::vector<int> Unit::getIndexesForUse(Unit* user) const {
	std::vector<int> indexes;
	if (belowCloseLimit() <= 0) { return indexes; }
	const int mainIndex = getMainBucketIndex();
	const std::vector<unsigned char>& closeTabIndexes = Game::getEnvironment()->getCloseTabIndexes(mainIndex);
	const std::vector<short>& closeIndexes = Game::getEnvironment()->getCloseIndexs(mainIndex);
	for (auto i : closeTabIndexes) {
		if (!ifSlotIsOccupied(i)) {
			int index = mainIndex + closeIndexes[i];
			if (Game::getEnvironment()->cellIsPassable(index)) {
				indexes.push_back(index);
			}
		}
	}

	return indexes;
}

std::vector<int> Unit::getIndexesForRangeUse(Unit* user) const {
	std::vector<int> indexes;
	if (belowRangeLimit() <= 0) { return indexes; }

	std::vector<int> allIndexes = Game::getEnvironment()->getIndexesInRange(
		getPosition(), user->getLevel()->rangeAttackRange);
	for (auto index : allIndexes) {

		if (canUse(index)
			&& std::ranges::find(surroundCells, index) == surroundCells.end()
			&& std::ranges::find(occupiedCells, index) == occupiedCells.end()) {
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

bool Unit::ifSlotIsOccupied(char index) const {
	assert(index < 8);
	assert(index >= 0);
	return useSockets & Flags::bitFlags[index];
}

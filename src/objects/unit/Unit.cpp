#include "Unit.h"
#include "DebugUnitType.h"
#include "Game.h"
#include "MathUtils.h"
#include "ObjectEnums.h"
#include "UnitOrder.h"
#include "aim/FutureAim.h"
#include "colors/ColorPeletteRepo.h"
#include "database/DatabaseCache.h"
#include "objects/PhysicalUtils.h"
#include "objects/unit/ChargeData.h"
#include "objects/unit/ColorMode.h"
#include "objects/unit/MissleData.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "scene/load/dbload_container.h"
#include "simulation/force/ForceStats.h"
#include "simulation/formation/FormationManager.h"
#include "state/StateManager.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <string>


Unit::Unit(Urho3D::Vector3* _position, int id, int player, int level) : Physical(_position, ObjectType::UNIT),
	state(UnitState::STOP) {
	initBillbords();

	dbUnit = Game::getDatabaseCache()->getUnit(id);
	dbLevel = Game::getDatabaseCache()->getUnitLevel(id, level).value();
	populate();
	if (StateManager::validateState(getDbID(), UnitState::CHARGE)) {
		chargeData = new ChargeData(150, 2);
	}

	if (StateManager::validateState(getDbID(), UnitState::SHOT)) {
		missleData = new MissleData(150, 2);
	}

	node->Scale(dbLevel->scale);
	model = node->CreateComponent<Urho3D::StaticModel>();
	model->SetModel(Game::getCache()->GetResource<Urho3D::Model>("Models/" + dbLevel->model));
	basic = Game::getCache()->GetResource<Urho3D::Material>("Materials/" + dbLevel->texture);
	model->SetMaterial(basic);

	setPlayer(player);
	setTeam(Game::getPlayersManager()->getPlayer(player)->getTeam());

	updateBillbords();
	if constexpr (UNIT_DEBUG_ENABLED) {
		line = node->GetOrCreateComponent<Urho3D::CustomGeometry>();
	}
	for (int& bucket : teamBucketIndex) {
		bucket = INT_MIN;
	}
}

Unit::~Unit() {
	delete chargeData;
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
	attackIntrest = dbLevel->attackRange * 10;
	rotatable = dbUnit->rotatable;
	actionState = UnitState(dbUnit->actionState);

	attackIntrest = 10;
	collectSpeed = dbLevel->collectSpeed;

	hpCoef = dbLevel->maxHp;
	maxHpCoef = dbLevel->maxHp;
	attackCoef = dbLevel->attack;
	attackRange = dbLevel->attackRange;
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

void Unit::actionIfCloseEnough(UnitState action, Physical* closest, float sqDistance,
                               float closeRange, float intrestRange) {
	if (closest) {
		if (sqDistance < closeRange * closeRange) {
			interactWithOne(closest, action);
		} else if (sqDistance < intrestRange * intrestRange) {
			addAim(FutureAim(closest, UnitOrder::FOLLOW));
		}
	}
}

void Unit::toAttack(std::vector<Physical*>* enemies) {
	auto [closest, minDistance] = closestPhysical(this, enemies, belowClose, exactPos);

	actionIfCloseEnough(UnitState::ATTACK, closest, minDistance, attackRange, attackIntrest);
}

void Unit::toShot(std::vector<Physical*>* enemies) {
	auto [closest, minDistance] = closestPhysical(this, enemies, belowRange, exactPos);

	actionIfCloseEnough(UnitState::SHOT, closest, minDistance, attackRange, attackIntrest);
}

void Unit::toCollect(std::vector<Physical*>* resources) {
	auto [closest, minDistance] = closestPhysical(this, resources, belowClose, posToFollow);
	//TODO dystans 0 jesli w odpowiednim bykecie
	actionIfCloseEnough(UnitState::COLLECT, closest, minDistance, attackRange, attackIntrest);
}

void Unit::interactWithOne(Physical* thing, UnitState action) {
	thingsToInteract.clear();
	thingsToInteract.push_back(thing);
	//gridIndexToInteract
	bool success = StateManager::changeState(this, action);
	if (!success) {
		thingsToInteract.clear();
	}
}

void Unit::toCharge(std::vector<Physical*>* enemies) {
	thingsToInteract.clear();
	for (auto entity : *enemies) {
		if (entity->isAlive()) {
			const float distance = sqDist(this, entity);
			if (distance <= chargeData->attackRange * chargeData->attackRange) {
				thingsToInteract.push_back(entity);
			}
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

void Unit::drawLine(Urho3D::CustomGeometry* line, const Urho3D::Vector3& first, const Urho3D::Vector3& second,
                    const Urho3D::Color& color = Urho3D::Color::WHITE) {
	line->DefineVertex(first / dbLevel->scale);
	line->DefineColor(color);
	line->DefineVertex(second / dbLevel->scale);
	line->DefineColor(color);
}

void Unit::debug(DebugUnitType type, ForceStats& stats) {
	if constexpr (UNIT_DEBUG_ENABLED) {
		if (billboardBar->enabled_) {

			line->Clear();
			line->SetNumGeometries(1);
			line->BeginGeometry(0, Urho3D::PrimitiveType::LINE_LIST);
			switch (type) {
			case DebugUnitType::NONE:
				break;
			case DebugUnitType::VELOCITY:
				drawLine(line, Urho3D::Vector3(0, 0.5, 0), Urho3D::Vector3(velocity.x_, 0.5, velocity.y_));
				break;
			case DebugUnitType::ACCELERATION:
				drawLine(line, Urho3D::Vector3(0, 0.5, 0), Urho3D::Vector3(acceleration.x_, 0.5, acceleration.y_));
				break;
			case DebugUnitType::SEPARATION_UNITS:
				drawLine(line, Urho3D::Vector3(0, 0.5, 0),
				         Urho3D::Vector3(stats.sepUnitLast.x_, 0.5, stats.sepUnitLast.y_));
				break;
			case DebugUnitType::SEPARATION_OBSTACLE:
				drawLine(line, Urho3D::Vector3(0, 0.5, 0),
				         Urho3D::Vector3(stats.sepObstLast.x_, 0.5, stats.sepObstLast.y_));
				break;
			case DebugUnitType::DESTINATION:
				drawLine(line, Urho3D::Vector3(0, 0.5, 0), Urho3D::Vector3(stats.destLast.x_, 0.5, stats.destLast.y_));
				break;
			case DebugUnitType::FORMATION:
				drawLine(line, Urho3D::Vector3(0, 0.5, 0), Urho3D::Vector3(stats.formLast.x_, 0.5, stats.formLast.y_));
				break;
			case DebugUnitType::ESCAPE:
				drawLine(line, Urho3D::Vector3(0, 0.5, 0), Urho3D::Vector3(stats.escaLast.x_, 0.5, stats.escaLast.y_));
				break;
			case DebugUnitType::ALL_FORCE:
				drawLine(line, Urho3D::Vector3(0, 0.5, 0), Urho3D::Vector3(velocity.x_, 0.5, velocity.y_));
				drawLine(line, Urho3D::Vector3(0, 0.5, 0),
				         Urho3D::Vector3(stats.sepUnitLast.x_, 0.5, stats.sepUnitLast.y_),
				         Urho3D::Color::RED);
				drawLine(line, Urho3D::Vector3(0, 0.5, 0),
				         Urho3D::Vector3(stats.sepObstLast.x_, 0.5, stats.sepObstLast.y_),
				         Urho3D::Color::GREEN);
				drawLine(line, Urho3D::Vector3(0, 0.5, 0), Urho3D::Vector3(stats.destLast.x_, 0.5, stats.destLast.y_),
				         Urho3D::Color::BLUE);
				drawLine(line, Urho3D::Vector3(0, 0.5, 0), Urho3D::Vector3(stats.formLast.x_, 0.5, stats.formLast.y_),
				         Urho3D::Color::YELLOW);
				drawLine(line, Urho3D::Vector3(0, 0.5, 0), Urho3D::Vector3(stats.escaLast.x_, 0.5, stats.escaLast.y_),
				         Urho3D::Color::CYAN);
				break;
			case DebugUnitType::AIM:
				if (aims.hasCurrent()) {
					auto lines = aims.getDebugLines(position);
					for (int i = 0; i < lines.size() - 1; ++i) {
						drawLine(line, lines[i], lines[i + 1]);
					}
				}
				break;
			default: ;
			}
			line->SetMaterial(Game::getColorPeletteRepo()->getLineMaterial());
			line->Commit();
		} else {
			line->Clear(); //TODO moze to zrobic tylko raz przy deslekcie
			line->Commit();
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

int Unit::getIndexToInteract() {
	return indexToInteract;
}

Urho3D::String& Unit::toMultiLineString() {
	menuString = dbUnit->name + " " + dbLevel->name;
	menuString.Append("\nAtak: ").Append(Urho3D::String(attackCoef))
	          .Append("\nObrona: ").Append(Urho3D::String(defenseCoef))
	          .Append("\nZdrowie: ").Append(Urho3D::String(hpCoef)).Append("/").Append(Urho3D::String(maxHpCoef));
	return menuString;
}

void Unit::action(char id, ActionParameter& parameter) {
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
	changeColor(Game::getColorPeletteRepo()->getColor(ColorPallet::RED, value, maxValue));
}

void Unit::changeColor(Urho3D::Material* newMaterial) const {
	if (newMaterial != model->GetMaterial(0)) {
		model->SetMaterial(newMaterial);
	}
}

void Unit::changeColor(ColorMode mode) {
	switch (mode) {
	case ColorMode::BASIC:
		changeColor(basic);
		break;
	case ColorMode::VELOCITY:
		changeColor(velocity.LengthSquared(), maxSpeed * maxSpeed);
		break;
	case ColorMode::STATE:
		changeColor(Game::getColorPeletteRepo()->getColor(state));
		break;
	case ColorMode::FORMATION:
		if (formation != -1) {
			changeColor(Game::getFormationManager()->getPriority(this), 3.0f);
		}
		break;
	default: ;
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
	state = UnitState(unit->state); //TODO nie wiem czy nie przepisaæpoprzez przejscie?
	//aimIndex =unit->aim_i;
	velocity = Urho3D::Vector2(unit->vel_x, unit->vel_z);
	//alive = unit->alive;
	hpCoef = maxHpCoef * unit->hp_coef;
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
		"velocity_z		INT     NOT NULL,"
		"aim_i		INT     NOT NULL";
}

void Unit::applyForce(double timeStep) {
	if (state == UnitState::ATTACK) {
		return;
	}

	velocity *= 0.5f; //TODO to dac jaki wspolczynnik tarcia terenu
	velocity += acceleration * (timeStep / mass);
	float velLenght = velocity.LengthSquared();
	if (velLenght < minSpeed * minSpeed) {
		if (state == UnitState::MOVE) {
			StateManager::changeState(this, UnitState::STOP);
		}
	} else {
		if (velLenght > maxSpeed * maxSpeed) {
			velocity.Normalize();
			velocity *= maxSpeed;
		}
		if (state == UnitState::STOP) {
			StateManager::changeState(this, UnitState::MOVE);
		}
		if (rotatable && velLenght > 2 * minSpeed * minSpeed) {
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

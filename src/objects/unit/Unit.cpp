#include "Unit.h"
#include "commands/action/ActionCommand.h"
#include "OrderType.h"
#include "database/DatabaseCache.h"
#include "Game.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <string>
#include <Urho3D/Resource/ResourceCache.h>
#include "commands/action/ActionCommandList.h"

double Unit::hbMaxSize = 0.7;

Unit::Unit(Vector3* _position, int id, int player) : Physical(_position, UNIT) {
	acceleration = new Vector3();
	velocity = new Vector3();
	toResource = new Vector3();
	aims = nullptr;
	resource = nullptr;

	unitState = UnitStateType::STOP;

	populate(Game::get()->getDatabaseCache()->getUnit(id));
	Model* model3d = Game::get()->getCache()->GetResource<Model>("Models/" + dbUnit->model);
	Material* material = Game::get()->getCache()->GetResource<Urho3D::Material>("Materials/" + dbUnit->texture);

	node->Scale(dbUnit->scale);
	StaticModel* model = node->CreateComponent<StaticModel>();
	model->SetModel(model3d);
	model->SetMaterial(material);

	setPlayer(player);
	setTeam(player);//TODO ustawic team

	initBillbords();
}

Unit::~Unit() {
	delete acceleration;
	delete velocity;
	delete toResource;
	if (aims) {
		aims->reduce();
	}
}

float Unit::getHealthBarSize() {
	double healthBarSize = hbMaxSize * (hpCoef / maxHpCoef);
	if (healthBarSize <= 0) { healthBarSize = 0; }
	return healthBarSize;
}

float Unit::getHealthPercent() {
	return (hpCoef / maxHpCoef);
}

void Unit::populate(db_unit* _dbUnit) {
	maxSeparationDistance = _dbUnit->maxSep;
	mass = _dbUnit->mass;
	maxSpeed = _dbUnit->maxSpeed;
	minSpeed = maxSpeed * 0.2f;
	minimalDistance = _dbUnit->minDist;
	attackRange = minimalDistance + 5;
	rotatable = _dbUnit->rotatable;
	actionState = UnitStateType(_dbUnit->actionState);
	dbUnit = _dbUnit;
}

void Unit::checkAim() {
	if (aims) {
		if (aims->ifReach(this, aimIndex)) {
			++aimIndex;
			if (aims->check(aimIndex)) {
				aims = nullptr;
				StateManager::get()->changeState(this, UnitStateType::MOVE);
			}
		}
	}
}


float Unit::getMinimalDistance() {
	return minimalDistance;
}

void Unit::move(double timeStep) {
	if (unitState != UnitStateType::STOP) {
		(*position) += (*velocity) * timeStep;
		node->SetPosition((*position));
		//node->Translate((*velocity) * timeStep, TS_WORLD);
	}
}

void Unit::setAcceleration(Vector3* _acceleration) {
	delete acceleration;
	_acceleration->y_ = 0;
	acceleration = _acceleration;
}

float Unit::getMaxSeparationDistance() {
	return maxSeparationDistance;
}

Vector3* Unit::getDestination(double boostCoef, double aimCoef) {
	if (aims) {
		auto dir = aims->getDirection(this, aimIndex);
		Vector3* force = nullptr;
		if (dir == nullptr) {
			if (!toResource) {
				force = new Vector3(*toResource);
			}
		} else {
			force = dir;
		}

		if (force) {

			force->Normalize();
			(*force) *= boostCoef;
			(*force) -= (*velocity);
			(*force) /= 0.5;
			(*force) *= mass;
			(*force) *= aimCoef;
			return force;
		}
	}

	return new Vector3();
}

Vector3* Unit::getVelocity() {
	return velocity;
}

float Unit::getUnitRadius() {
	return unitRadius;
}

void Unit::absorbAttack(double attackCoef) {
	hpCoef -= attackCoef * (1 - defenseCoef);
	if (billboardBar->enabled_) {
		updateHealthBar();
	}
}

void Unit::attackIfCloseEnough(double& distance, Unit* closest) {
	if (closest) {
		if (distance < attackRange * attackRange) {
			toAttack(closest);
			//attackRange();
		} else if (distance < attackIntrest * attackIntrest) {
			Game::get()->getActionCommandList()->add(new ActionCommand(this, OrderType::FOLLOW, closest));
		}
	}
}

void Unit::collectIfCloseEnough(double distance, ResourceEntity* closest) {
	if (closest) {
		if (distance < attackRange * attackRange) {
			toCollect(closest);
		} else if (distance < attackIntrest * attackIntrest) {
			Game::get()->getActionCommandList()->add(new ActionCommand(this, OrderType::FOLLOW, closest));
		}
	}
}

void Unit::toAttack(std::vector<Unit*>* enemies) {
	double minDistance = 9999;
	Unit* entityClosest = nullptr;
	for (auto entity : (*enemies)) {
		double distance = (*this->getPosition() - *entity->getPosition()).LengthSquared();
		if (distance <= minDistance) {
			minDistance = distance;
			entityClosest = entity;
		}
	}
	attackIfCloseEnough(minDistance, entityClosest);
}

void Unit::toAttack(Physical* enemy) {
	StateManager::get()->changeState(this, UnitStateType::ATTACK);
	enemyToAttack = enemy;
}

void Unit::toAttack() {
	toAttack(enemyToAttack);
}

void Unit::toCollect(std::vector<Physical*>* enemies) {
	double minDistance = 9999;
	ResourceEntity* entityClosest = nullptr;
	for (auto entity : (*enemies)) {
		ResourceEntity* resource = static_cast<ResourceEntity*>(entity);
		if (resource->belowLimit()) {
			const double distance = (*this->getPosition() - *entity->getPosition()).LengthSquared();
			if (distance <= minDistance) {
				minDistance = distance;
				entityClosest = resource;
			}
		}
	}
	collectIfCloseEnough(minDistance, entityClosest);
}

void Unit::toCollect() {
	return toCollect(resource);
}

void Unit::toCollect(ResourceEntity* _resource) {
	StateManager::get()->changeState(this, UnitStateType::COLLECT);
	resource = _resource;
}

void Unit::updateHeight(double y, double timeStep) {
	double diff = position->y_ - y;

	(*velocity) *= 1 + diff * mass * timeStep;
	position->y_ = y;
}

void Unit::addAim(ActionParameter* actionParameter) {
	if (actionParameter->getAims() != aims) {
		removeAim();
		aims = actionParameter->getAims();
		aims->up();
	}
}

void Unit::removeAim() {
	if (aims != nullptr) {
		aims->reduce();
		aims = nullptr;
	}
	aimIndex = 0;
}

String& Unit::toMultiLineString() {
	menuString = dbUnit->name;
	menuString.Append("\nAtak: ").Append(String(attackCoef));
	menuString.Append("\nObrona: ").Append(String(defenseCoef));
	menuString.Append("\nZdrowie: ").Append(String(hpCoef)).Append("/").Append(String(maxHpCoef));
	return menuString;
}

void Unit::action(short id, ActionParameter* parameter) {
	OrderType type = OrderType(id);

	switch (type) {
	case OrderType::GO:
		StateManager::get()->changeState(this, UnitStateType::GO, parameter);
		break;
	case OrderType::STOP:
		StateManager::get()->changeState(this, UnitStateType::STOP);
		break;
	case OrderType::CHARGE:
		StateManager::get()->changeState(this, UnitStateType::CHARAGE, parameter);
		break;
	case OrderType::ATTACK: break;
	case OrderType::PATROL:
		StateManager::get()->changeState(this, UnitStateType::PATROL, parameter);
		break;
	case OrderType::DEAD:
		StateManager::get()->changeState(this, UnitStateType::DEAD);
		break;
	case OrderType::DEFEND:
		StateManager::get()->changeState(this, UnitStateType::DEFEND);
		break;
	case OrderType::FOLLOW:
		StateManager::get()->changeState(this, UnitStateType::FOLLOW, parameter);
		break;
	default: ;
	}
}

std::string Unit::getValues(int precision) {
	int position_x = position->x_ * precision;
	int position_z = position->z_ * precision;
	int state = (int)unitState;
	int velocity_x = velocity->x_ * precision;
	int velocity_z = velocity->z_ * precision;
	return Physical::getValues(precision)
		+ to_string(position_x) + "," +
		to_string(position_z) + "," +
		to_string(state) + "," +
		to_string(velocity_x) + "," +
		to_string(velocity_z) + "," +
		to_string(aimIndex);

}

UnitStateType Unit::getState() {
	return unitState;
}

UnitStateType Unit::getActionState() {
	return actionState;
}

void Unit::clean() {
	if (resource != nullptr && !resource->isAlive()) {
		resource = nullptr;
	}
	Physical::clean();
}

void Unit::setState(UnitStateType state) {
	unitState = state;
}

bool Unit::checkTransition(UnitStateType state) {
	return StateManager::get()->checkChangeState(this, state);
}

void Unit::executeState() {
	StateManager::get()->execute(this);
}

bool Unit::hasResource() {
	return resource;
}

void Unit::load(dbload_unit* unit) {
	unitState = UnitStateType(unit->state);//TODO nie wiem czy nie przepisaæpoprzez przejscie?
	//aimIndex =unit->aim_i;
	velocity->x_ = unit->vel_x;
	velocity->z_ = unit->vel_z;
	alive = unit->alive;
	hpCoef = maxHpCoef * unit->hp_coef;
}

std::string Unit::getColumns() {
	return Physical::getColumns() +
		"position_x		INT     NOT NULL,"
		"position_z		INT     NOT NULL,"
		"state			INT     NOT NULL,"
		"velocity_x		INT     NOT NULL,"//TODO czy dodac y?
		"velocity_z		INT     NOT NULL,"
		"aim_i		INT     NOT NULL";
}

void Unit::applyForce(double timeStep) {
	if (unitState == UnitStateType::ATTACK) {
		(*velocity) = Vector3::ZERO;
		return;
	}

	(*velocity) *= 0.95;//TODO to dac jaki wspolczynnik tarcia terenu
	(*velocity) += (*acceleration) * (timeStep / mass);
	double velLenght = velocity->LengthSquared();
	if (velLenght < minSpeed * minSpeed) {
		StateManager::get()->changeState(this, UnitStateType::STOP);
	} else {
		if (velLenght > maxSpeed * maxSpeed) {
			velocity->Normalize();
			(*velocity) *= maxSpeed;
		}
		StateManager::get()->changeState(this, UnitStateType::MOVE);
		if (rotatable) {
			rotation->x_ = velocity->x_;
			rotation->z_ = velocity->z_;
			node->SetDirection(*rotation);
		}
	}
}

int Unit::getDbID() {
	return dbUnit->id;
}

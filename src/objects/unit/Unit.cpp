#include "Unit.h"
#include "commands/ActionCommand.h"
#include "OrderType.h"

double Unit::hbMaxSize = 1.0;

Unit::Unit(Vector3* _position, Urho3D::Node* _boxNode) : Physical(_position, _boxNode, UNIT) {
	acceleration = new Vector3();
	velocity = new Vector3();
	aims = nullptr;

	followTo = nullptr;
	unitState = UnitStateType::STOP;
	node->SetPosition(*_position);

}

Unit::~Unit() {
	delete acceleration;
	delete velocity;
	followTo = nullptr;
	if (aims) {
		aims->reduce();
	}
}

double Unit::getHealthBarSize() {
	double healthBarSize = hbMaxSize * (hpCoef / maxHpCoef);
	if (healthBarSize <= 0) { healthBarSize = 0; }
	return healthBarSize;
}

void Unit::populate(db_unit* _dbUnit, StateManager* _states) {
	maxSeparationDistance = _dbUnit->maxSep;
	mass = _dbUnit->mass;
	maxSpeed = _dbUnit->maxSpeed;
	minSpeed = maxSpeed * 0.2f;
	minimalDistance = _dbUnit->minDist;
	attackRange = minimalDistance + 2;
	textureName = "Materials/" + String(_dbUnit->texture);
	unitType = UnitType(_dbUnit->type);
	rotatable = _dbUnit->rotatable;

	states = _states;

	dbUnit = _dbUnit;
}

void Unit::checkAim() {
	if (aims) {
		if (aims->ifReach(position, aimIndex)) {
			++aimIndex;
			if (aims->check(aimIndex)) {
				aims = nullptr;
				states->changeState(this, UnitStateType::MOVE);
			}
		}
	} else if (followTo) {
		if (((*position) - (*followTo->getPosition())).LengthSquared() <= 3 * 3) {
			followTo = nullptr;
			states->changeState(this, UnitStateType::MOVE);
		}
	}
}

void Unit::move(double timeStep) {
	(*position) += (*velocity) * timeStep;
	node->Translate((*velocity) * timeStep, TS_WORLD);
}

void Unit::setAcceleration(Vector3* _acceleration) {
	delete acceleration;
	acceleration = _acceleration;
}

double Unit::getMaxSeparationDistance() {
	return maxSeparationDistance;
}

void Unit::action(ActionType actionType, ActionParameter* parameter) {
	switch (actionType) {
	case ADD_AIM:
		states->changeState(this, UnitStateType::GO, parameter);
		break;
	case APPEND_AIM:
		states->changeState(this, UnitStateType::PATROL, parameter);
		break;
	case FOLLOW:
		states->changeState(this, UnitStateType::FOLLOW, parameter);
		break;
	default:
		break;//zalogowaæ
	}
}

Vector3* Unit::getAim() {
	if (aims) {
		return aims->getAimPos(aimIndex);
	}
	if (followTo) {
		return followTo->getPosition();
	}

	return nullptr;
}

Vector3* Unit::getVelocity() {
	return velocity;
}

double Unit::getMass() {
	return mass;
}

double Unit::getUnitRadius() {
	return unitRadius;
}

void Unit::absorbAttack(double attackCoef) {
	hpCoef -= attackCoef * (1 - defenseCoef);
	if (billboardBar->enabled_) {
		updateHealthBar();
	}
}

void Unit::attackIfCloseEnough(double& distance, Physical* closest) {
	if (closest) {
		if (distance < attackRange * attackRange) {
			attack(closest);
			//attackRange();
		} else if (distance < attackIntrest * attackIntrest) {
			ActionCommand* command = new ActionCommand(this, FOLLOW, closest);
			Game::get()->getActionCommandList()->add(command);
		}
	}
}

void Unit::attack(vector<Physical*>* enemies) {
	double minDistance = 9999;
	Physical* entityClosest = nullptr;
	for (auto entity : (*enemies)) {
		double distance = (*this->getPosition() - *entity->getPosition()).LengthSquared();
		if (distance <= minDistance) {
			minDistance = distance;
			entityClosest = entity;
		}
	}
	attackIfCloseEnough(minDistance, entityClosest);
}

void Unit::attack(Physical* enemy) {
	states->changeState(this, UnitStateType::ATTACK);
	enemy->absorbAttack(attackCoef);
	enemyToAttack = enemy;
}

void Unit::attack() {
	attack(enemyToAttack);
}

void Unit::updateHeight(double y, double timeStep) {
	double diff = position->y_ - y;
	(*velocity) *= 1 + diff * mass * timeStep;
	position->y_ = y;
}

void Unit::appendAim(ActionParameter* actionParameter) {
	if (actionParameter->getAims() != aims && aims != nullptr) {
		aims->reduce();
		aimIndex = 0;
		aims = actionParameter->getAims();
		aims->up();
		followTo = nullptr;
	} else if (aims == nullptr) {
		aimIndex = 0;
		aims = actionParameter->getAims();
		aims->up();
		followTo = nullptr;
	}
}

void Unit::addAim(ActionParameter* actionParameter) {
	if (actionParameter->getAims() != aims && aims != nullptr) {
		aims->reduce();
	}
	aimIndex = 0;
	aims = actionParameter->getAims();
	aims->up();
	followTo = nullptr;
}

void Unit::removeAim() {
	if (aims != nullptr) {
		aims->reduce();
		aims = nullptr;
	}
	aimIndex = 0;
	followTo = nullptr;
}

void Unit::followAim(ActionParameter* parameter) {
	removeAim();
	followTo = parameter->getFollowTo();
}

void Unit::updateRotation() {
	if (rotatable) {
		node->SetDirection(*rotation);
	}
}

String* Unit::toMultiLineString() {
	(*menuString) = dbUnit->name;
	(*menuString) += "\nAtak: " + String(attackCoef);
	(*menuString) += "\nObrona: " + String(defenseCoef);
	(*menuString) += "\nZdrowie: " + String(hpCoef) + "/" + String(maxHpCoef);
	return menuString;
}

void Unit::buttonAction(short id) {
	OrderType type = OrderType(id);

	switch (type) {
	case OrderType::GO: break;
	case OrderType::STOP:
		states->changeState(this, UnitStateType::STOP);
		break;
	case OrderType::CHARGE: break;
	case OrderType::ATTACK: break;
	case OrderType::PATROL: break;
	case OrderType::DEAD:
		states->changeState(this, UnitStateType::DEAD);
		break;
	case OrderType::DEFEND:
		states->changeState(this, UnitStateType::DEFEND);
		break;
	default: ;
	}
}

UnitStateType Unit::getState() {
	return unitState;
}

void Unit::clean() {
	if (followTo != nullptr && !followTo->isAlive()) {
		followTo = nullptr;
	}
	Physical::clean();
}

void Unit::setState(UnitStateType state) {
	this->unitState = state;
}

bool Unit::checkTransition(UnitStateType state) {
	return states->checkChangeState(this, state);
}

void Unit::applyForce(double timeStep) {
	if (unitState == UnitStateType::ATTACK) {
		(*velocity) = Vector3::ZERO;
		return;
	}
	double coef = timeStep / mass;
	(*velocity) *= 0.95;//TODO to dac jaki wspolczynnik tarcia terenu
	(*velocity) += (*acceleration) * coef;
	double velLenght = velocity->LengthSquared();
	if (velLenght < minSpeed * minSpeed) {
		(*velocity) = Vector3::ZERO;//TODO dac to do zmiany stanu?
		states->changeState(this, UnitStateType::STOP);//TODO ERROR to moze byc zle bo co gdy jest GO a val 
	} else {
		if (velLenght > maxSpeed * maxSpeed) {
			velocity->Normalize();
			(*velocity) *= maxSpeed;
			states->changeState(this, UnitStateType::MOVE);
		}
		rotation->x_ = velocity->x_;
		rotation->z_ = velocity->z_;
	}
}

int Unit::getSubTypeId() {
	return dbUnit->id;
}

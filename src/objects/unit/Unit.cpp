#include "Unit.h"
#include "commands/ActionCommand.h"
#include "OrderType.h"

double Unit::hbMaxSize = 1.0;
StateManager* Unit::states = nullptr;

Unit::Unit(Vector3* _position, Urho3D::Node* _boxNode) : Physical(_position, _boxNode, UNIT) {
	acceleration = new Vector3();
	velocity = new Vector3();
	aims = nullptr;

	unitState = UnitStateType::STOP;
	node->SetPosition(*_position);
}

Unit::~Unit() {
	delete acceleration;
	delete velocity;
	if (aims) {
		aims->reduce();
	}
}

double Unit::getHealthBarSize() {
	double healthBarSize = hbMaxSize * (hpCoef / maxHpCoef);
	if (healthBarSize <= 0) { healthBarSize = 0; }
	return healthBarSize;
}

void Unit::populate(db_unit* _dbUnit) {
	maxSeparationDistance = _dbUnit->maxSep;
	mass = _dbUnit->mass;
	maxSpeed = _dbUnit->maxSpeed;
	minSpeed = maxSpeed * 0.2f;
	minimalDistance = _dbUnit->minDist;
	attackRange = minimalDistance + 2;
	textureName = "Materials/" + String(_dbUnit->texture);
	unitType = UnitType(_dbUnit->type);
	rotatable = _dbUnit->rotatable;

	dbUnit = _dbUnit;
}

void Unit::checkAim() {
	if (aims) {
		if (aims->ifReach(this, aimIndex)) {
			++aimIndex;
			if (aims->check(aimIndex)) {
				aims = nullptr;
				states->changeState(this, UnitStateType::MOVE);
			}
		}
	}
}

void Unit::move(double timeStep) {
	if (unitState != UnitStateType::STOP) {
		(*position) += (*velocity) * timeStep;
		node->Translate((*velocity) * timeStep, TS_WORLD);
	}
}

void Unit::setAcceleration(Vector3* _acceleration) {
	delete acceleration;
	acceleration = _acceleration;
}

double Unit::getMaxSeparationDistance() {
	return maxSeparationDistance;
}

Vector3* Unit::getDestination() {
	if (aims) {
		return aims->getDirection(this, aimIndex);
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
			Game::get()->getActionCommandList()->add(new ActionCommand(this, OrderType::FOLLOW, closest));
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

void Unit::addAim(ActionParameter* actionParameter) {
	if (actionParameter->getAims() != aims) {
		if (aims != nullptr) {
			aims->reduce();
		}

		aimIndex = 0;
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

String* Unit::toMultiLineString() {
	(*menuString) = dbUnit->name;
	(*menuString) += "\nAtak: " + String(attackCoef);
	(*menuString) += "\nObrona: " + String(defenseCoef);
	(*menuString) += "\nZdrowie: " + String(hpCoef) + "/" + String(maxHpCoef);
	return menuString;
}

void Unit::action(short id, ActionParameter* parameter) {
	OrderType type = OrderType(id);

	switch (type) {
	case OrderType::GO:
		states->changeState(this, UnitStateType::GO, parameter);
		break;
	case OrderType::STOP:
		states->changeState(this, UnitStateType::STOP);
		break;
	case OrderType::CHARGE:
		states->changeState(this, UnitStateType::CHARAGE, parameter);
		break;
	case OrderType::ATTACK: break;
	case OrderType::PATROL:
		states->changeState(this, UnitStateType::PATROL, parameter);
		break;
	case OrderType::DEAD:
		states->changeState(this, UnitStateType::DEAD);
		break;
	case OrderType::DEFEND:
		states->changeState(this, UnitStateType::DEFEND);
		break;
	case OrderType::FOLLOW:
		states->changeState(this, UnitStateType::FOLLOW, parameter);
		break;
	default: ;
	}
}

UnitStateType Unit::getState() {
	return unitState;
}

void Unit::clean() {
	//TODO pamietac zeby wyczyscic followTO
	Physical::clean();
}

void Unit::setState(UnitStateType state) {
	unitState = state;
}

bool Unit::checkTransition(UnitStateType state) {
	return states->checkChangeState(this, state);
}

void Unit::executeState() {
	states->execute(this);
}

void Unit::setStates(StateManager* _states) {
	Unit::states = _states;
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
		states->changeState(this, UnitStateType::STOP);
	} else {
		if (velLenght > maxSpeed * maxSpeed) {
			velocity->Normalize();
			(*velocity) *= maxSpeed;
		}
		states->changeState(this, UnitStateType::MOVE);
		if (rotatable) {
			rotation->x_ = velocity->x_;
			rotation->z_ = velocity->z_;
			node->SetDirection(*rotation);
		}
	}
}

int Unit::getSubTypeId() {
	return dbUnit->id;
}

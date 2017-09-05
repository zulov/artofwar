#include "Unit.h"
#include "commands/ActionCommand.h"
#include "OrderType.h"

double Unit::hbMaxSize = 1.0;

Unit::Unit(Vector3* _position, Urho3D::Node* _boxNode) : Physical(_position, _boxNode, UNIT) {
	acceleration = new Vector3();
	velocity = new Vector3();
	aims = nullptr;

	followTo = nullptr;
	unitState = US_STOP;
	node->SetPosition(*_position);
	states = new State*[STATE_SIZE];
	for (int i = 0; i < STATE_SIZE; ++i) {
		states[i] = nullptr;
	}

	//states[0] = new StopState(this);
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
	if (aims != nullptr) {
		bool reach = aims->ifReach(position, aimIndex);
		if (reach) {
			aimIndex++;
			bool end = aims->check(aimIndex);
			if (end) {
				aims = nullptr;
			}
		}
	} else if (followTo != nullptr) {//TODO a co jak cel umrze?
		bool reach = false;

		double distance = ((*position) - (*followTo->getPosition())).Length();
		if (distance <= 3) {
			reach = true;
		}

		if (reach) {
			followTo = nullptr;
		}
	}
}

void Unit::move(double timeStep) {
	(*position) += (*velocity) * timeStep;
	//node->SetPosition(*position);
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
		addAim(parameter);
		break;
	case APPEND_AIM:
		appendAim(parameter);
		break;
	case FOLLOW:
		followAim(parameter);
		break;
	default:
		break;//zalogowaæ
	}
}

Vector3* Unit::getAim() {
	if (aims) {
		unitState = US_GOTO;
		return aims->getAimPos(aimIndex);
	}
	if (followTo) {
		return followTo->getPosition();
	}
	if (unitState == US_GOTO) {
		unitState = US_STOP;
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
	for (int j = 0; j < enemies->size(); ++j) {
		Physical* entity = (*enemies)[j];
		double distance = (*this->getPosition() - *entity->getPosition()).LengthSquared();
		if (distance <= minDistance) {
			minDistance = distance;
			entityClosest = entity;
		}
	}
	attackIfCloseEnough(minDistance, entityClosest);

}

void Unit::attack(Physical* enemy) {
	unitState = US_ATTACK;
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
		removeAim();
		unitState = UnitStateType::US_STOP;
		break;
	case OrderType::CHARGE: break;
	case OrderType::ATTACK: break;
	case OrderType::PATROL: break;
	case OrderType::DEAD:
		alive = false;
		break;
	case OrderType::DEFEND:
		removeAim();
		unitState = UnitStateType::US_DEFEND;
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

void Unit::applyForce(double timeStep) {
	if (unitState == US_ATTACK) {
		velocity->x_ = 0;
		velocity->y_ = 0;
		velocity->z_ = 0;
		return;
	}
	double coef = timeStep / mass;
	(*velocity) *= 0.95;//TODO to dac jaki wspolczynnik tarcia terenu
	(*velocity) += (*acceleration) * coef;
	double velLenght = velocity->LengthSquared();
	if (velLenght > maxSpeed * maxSpeed) {
		velocity->Normalize();
		(*velocity) *= maxSpeed;
		if (unitState != US_GOTO) {
			unitState = US_MOVE;
		}

	} else if (velLenght < minSpeed * minSpeed) {
		velocity->x_ = 0;
		velocity->y_ = 0;
		velocity->z_ = 0;
		///velocity->ZERO;
		if (unitState != US_GOTO) {
			unitState = US_STOP;
		}
	} else {
		rotation->x_ = velocity->x_;
		rotation->z_ = velocity->z_;
	}
}

int Unit::getSubTypeId() {
	return dbUnit->id;
}

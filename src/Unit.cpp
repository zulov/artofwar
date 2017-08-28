#include "Unit.h"
#include "ActionCommand.h"

double Unit::hbMaxSize = 1.0;

Unit::Unit(Vector3* _position, Urho3D::Node* _boxNode) : Physical(_position, _boxNode, UNIT) {
	acceleration = new Vector3();
	velocity = new Vector3();
	aims = nullptr;

	aimPosition = nullptr;
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
	aimPosition = nullptr;
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

void Unit::move(double timeStep) {
	(*position) += (*velocity) * timeStep;
	//node->SetPosition(*position);
	node->Translate((*velocity) * timeStep, TS_WORLD);
	if (aims != nullptr) {
		bool reach = aims->ifReach(position, aimIndex);
		if (reach) {
			aimIndex++;
			bool end = aims->check(aimIndex);
			if (end) {
				aims = nullptr;
			}
		}
	} else if (aimPosition != nullptr) {//TODO a co jak cel umrze?
		bool reach = false;

		double distance = ((*position) - (*aimPosition)).Length();
		if (distance <= 3) {
			reach = true;
		}

		if (reach) {
			aimPosition = nullptr;
		}
	}
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
	if (aimPosition) {
		return aimPosition;
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

void Unit::attack(vector<Physical*>* enemies) {
	if (unitState == US_STOP || unitState == US_ATTACK) {
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
		if (entityClosest) {
			minDistance = sqrt(minDistance);
			if (minDistance < attackRange) {
				attack(entityClosest);
				unitState = US_ATTACK;
				//attackRange();
			} else if (minDistance < attackIntrest) {
				ActionCommand* command = new ActionCommand(this, FOLLOW, entityClosest->getPosition());
				Game::get()->getActionCommandList()->add(command);
			}
		}
	}
}

void Unit::attack(Physical* enemy) {
	enemy->absorbAttack(attackCoef);
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
		aimPosition = nullptr;
	} else if (aims == nullptr) {
		aimIndex = 0;
		aims = actionParameter->getAims();
		aims->up();
		aimPosition = nullptr;
	}
}

void Unit::addAim(ActionParameter* actionParameter) {
	if (actionParameter->getAims() != aims && aims != nullptr) {
		aims->reduce();
	}
	aimIndex = 0;
	aims = actionParameter->getAims();
	aims->up();
	aimPosition = nullptr;
}

void Unit::followAim(ActionParameter* parameter) {
	aimPosition = parameter->getAimPosition();
	aimIndex = 0;
	if (aims != nullptr) {
		aims->reduce();
		aims = nullptr;
	}
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

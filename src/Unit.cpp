#include "Unit.h"
#include "ActionCommand.h"


Unit::Unit(Vector3* _position, Urho3D::Node* _boxNode, Font* _font) : Entity(_position, _boxNode, _font) {
	acceleration = new Vector3();
	velocity = new Vector3();
	aims = nullptr;
	healthBar = nullptr;
	aimPosition = nullptr;
	unitState = US_STOP;
}

Unit::~Unit() {
	//delete node;
	//delete aims;
}

void Unit::populate(db_unit* definition) {
	maxSeparationDistance = definition->maxSep;
	mass = definition->mass;
	maxSpeed = definition->maxSpeed;
	minSpeed = maxSpeed * 0.2f;
	minimalDistance = definition->minDist;
	attackRange = minimalDistance + 2;
	textureName = "Materials/" + definition->texture;
}

void Unit::move(double timeStep) {
	(*position) += (*velocity) * timeStep;
	node->SetPosition(*position);

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
	if (healthBar) {
		double healthBarSize = 2 * (hpCoef / maxHpCoef);
		if (healthBarSize <= 0) { healthBarSize = 0; }
		healthBar->SetScale(Vector3(healthBarSize, 0.3, 0.3f));
	}
}

void Unit::attack(vector<Entity*>* enemies) {
	if (unitState == US_STOP || unitState == US_ATTACK) {
		double minDistance = 9999;
		Entity* entityClosest = nullptr;
		for (int j = 0; j < enemies->size(); ++j) {
			Entity* entity = (*enemies)[j];
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

void Unit::attack(Entity* enemy) {
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
		if (unitState != US_GOTO) {
			unitState = US_STOP;
		}
	} else {

		rotation->x_ = velocity->x_;
		rotation->y_ = velocity->y_;
		rotation->z_ = velocity->z_;
	}
}

ObjectType Unit::getType() {
	return UNIT;
}

void Unit::select() {
//	Node* title = node->CreateChild("title");
//	title->SetPosition(Vector3(0.0f, 1.5f, 0.0f));
//	Text3D* titleText = title->CreateComponent<Text3D>();
//	titleText->SetText("Entity");
//
//	titleText->SetFont(font, 24);
//	titleText->SetColor(Color::GREEN);
//	titleText->SetAlignment(HA_CENTER, VA_CENTER);
//	titleText->SetFaceCameraMode(FC_LOOKAT_MIXED);

	StaticModel* model = node->GetComponent<StaticModel>();
	model->SetMaterial(Game::get()->getCache()->GetResource<Urho3D::Material>("Materials/green.xml"));


	healthBar = node->CreateChild("healthBar");
	double healthBarSize = 2 * (hpCoef / maxHpCoef);
	if (healthBarSize <= 0) { healthBarSize = 0; }
	healthBar->SetScale(Vector3(healthBarSize, 0.3, 0.3f));
	healthBar->SetPosition(Vector3(0, 1.2f, 0));
	healthBar->Pitch(-70);

	StaticModel* planeObject = healthBar->CreateComponent<StaticModel>();
	planeObject->SetModel(Game::get()->getCache()->GetResource<Model>("Models/Cube.mdl"));
	planeObject->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/red.xml"));
}

void Unit::unSelect() {
//	Node* child = node->GetChild("title");
//	if (child) {
//		child->RemoveAllChildren();
//		node->RemoveChild(child);
//	}

	Node* child1 = healthBar;
	if (child1) {
		child1->RemoveAllChildren();
		node->RemoveChild(child1);
		healthBar = nullptr;
	}

	StaticModel* model = node->GetComponent<StaticModel>();
	model->SetMaterial(Game::get()->getCache()->GetResource<Urho3D::Material>(textureName));
}

#include "Unit.h"
#include "ObjectEnums.h"
#include "Main.h"
#include "Game.h"

Unit::Unit(Vector3* _position, Urho3D::Node* _boxNode, Font* _font) : Entity(_position, _boxNode, _font) {
	maxSeparationDistance = SEP_RADIUS;
	mass = 1;
	maxSpeed = 5;
	minSpeed = maxSpeed * 0.2f;
	acceleration = new Vector3();
	velocity = new Vector3();
	aims = new Aims();
}

Unit::~Unit() {
	//delete node;
	delete aims;
}

void Unit::move(double timeStep) {
	(*position) += (*velocity) * timeStep;
	node->SetPosition(*position);
	(*velocity) *= 0.95;
	aims->check(position);
}

void Unit::setAcceleration(Vector3* _acceleration) {
	delete acceleration;
	acceleration = _acceleration;
}

double Unit::getMaxSeparationDistance() {
	return maxSeparationDistance;
}

void Unit::action(ActionType actionType, Entity* entity) {
	switch (actionType) {
	case ADD_AIM:
		addAim(entity);
		break;
	case FOLLOW:
		break;
	default:
		break;//zalogowaæ
	}
}

Vector3* Unit::getAim() {
	return aims->getAimPos();
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

void Unit::addAim(Entity* entity) {
	aims->add(entity);
}

void Unit::applyForce(double timeStep) {
	double coef = timeStep / mass;
	(*velocity) += (*acceleration) * coef;
	double velLenght = velocity->LengthSquared();
	if (velLenght > maxSpeed * maxSpeed) {
		velocity->Normalize();
		(*velocity) *= maxSpeed;
	} else if (velLenght > 0 && velLenght < minSpeed * minSpeed) {
		velocity->x_ = 0;
		velocity->y_ = 0;
		velocity->z_ = 0;
	}
}

int Unit::getType() {
	return UNIT;
}

void Unit::select() {
	Node* title = node->CreateChild("title");
	title->SetPosition(Vector3(0.0f, 1.2f, 0.0f));
	Text3D* titleText = title->CreateComponent<Text3D>();
	titleText->SetText("Entity");

	titleText->SetFont(font, 24);
	titleText->SetColor(Color::GREEN);
	titleText->SetAlignment(HA_CENTER, VA_CENTER);
	titleText->SetFaceCameraMode(FC_LOOKAT_MIXED);
	StaticModel* model = node->GetComponent<StaticModel>();

	model->SetMaterial(Game::getInstance()->getCache()->GetResource<Urho3D::Material>("Materials/green.xml"));
}

void Unit::unSelect() {
	Node* child = node->GetChild("title");
	if (child) {
		node->RemoveChild(child);
	}

	StaticModel* model = node->GetComponent<StaticModel>();
	model->SetMaterial(nullptr);//SetMaterial(Game::getInstance()->getCache()->GetResource<Urho3D::Material>("Materials/green.xml"));
}

#include "Unit.h"
#include "ObjectEnums.h"

Unit::Unit(Vector3 _position, Urho3D::Node* _boxNode, Font* _font) : Entity(_position, _boxNode, _font) {
	maxSeparationDistance = 4;
	mass = 1;
	maxSpeed = 2;
	acceleration = Vector3();
	velocity = Vector3();
}

Unit::~Unit() {
	//delete node;
}

void Unit::move(double timeStep) {
	position += velocity * timeStep;
	node->SetPosition(position);
	velocity *= 0.95;
}

void Unit::setAcceleration(Vector3 _acceleration) {
	acceleration = _acceleration;
}

double Unit::getMaxSeparationDistance() {
	return maxSeparationDistance;
}

void Unit::applyForce(double timeStep) {
	double coef = timeStep / mass;
	velocity += acceleration * coef;
	if (velocity.Length() > maxSpeed * maxSpeed) {
		velocity.Normalize();
		velocity *= maxSpeed;
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
}

void Unit::unSelect() {
	Node* child = node->GetChild("title");
	if (child) {
		node->RemoveChild(child);
	}

}

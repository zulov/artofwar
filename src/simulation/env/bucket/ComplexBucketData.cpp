#include "ComplexBucketData.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include "Game.h"
#include "objects/Static.h"


ComplexBucketData::ComplexBucketData() {
	box = nullptr;
	removeStatic();
}


ComplexBucketData::~ComplexBucketData() {
	box->Remove();
}


ObjectType ComplexBucketData::getType() {
	return type;
}

void ComplexBucketData::setStatic(Static* _object) {
	object = _object;
	type = object->getType();

	if (box) {
		model->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/red_alpha.xml"));
	}
}

void ComplexBucketData::removeStatic() {
	object = nullptr;
	type = ObjectType::UNIT;
	if (box) {
		model->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/blue_alpha.xml"));
	}
}


void ComplexBucketData::createBox(double bucketSize) {
	box = Game::get()->getScene()->CreateChild();
	box->SetPosition(Vector3(center.x_, 0, center.y_));
	box->Scale(Vector3(bucketSize * 0.8, 1, bucketSize * 0.8));
	box->Translate(Vector3::UP * 10, TS_PARENT);
	model = box->CreateComponent<StaticModel>();
	model->SetModel(Game::get()->getCache()->GetResource<Model>("Models/plane.mdl"));
	model->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/blue_alpha.xml"));
	box->SetEnabled(true);
}

Vector3* ComplexBucketData::getDirectrionFrom(Vector3* position) {
	return new Vector3(center.x_ - position->x_, 0, center.y_ - position->z_);
}

void ComplexBucketData::setCenter(double _centerX, double _centerY) {
	center = Vector2(_centerX, _centerY);
}

Urho3D::Vector2 ComplexBucketData::getCenter() {
	return center;
}

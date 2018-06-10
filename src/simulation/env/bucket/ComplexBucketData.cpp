#include "ComplexBucketData.h"
#include "Game.h"
#include "objects/Static.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <iostream>


ComplexBucketData::ComplexBucketData(): box(nullptr) {
	removeStatic();
	neighbours.reserve(8);
}

ComplexBucketData::~ComplexBucketData() {
	if (box) {
		box->Remove();
	}
}

void ComplexBucketData::setStatic(Static* _object) {
	object = _object;
	type = object->getType();

	if (box) {
		Urho3D::StaticModel* model = box->GetComponent<StaticModel>();
		model->SetMaterial(Game::getCache()->GetResource<Material>("Materials/red_alpha.xml"));
	}
	if (_object->getType() == ObjectType::BUILDING) {
		additonalInfo = _object->getPlayer();
	} else {
		additonalInfo = _object->getDbID();
	}
}

void ComplexBucketData::removeStatic() {
	object = nullptr;
	type = ObjectType::UNIT;
	if (box) {
		auto model = box->GetComponent<StaticModel>();
		model->SetMaterial(Game::getCache()->GetResource<Material>("Materials/blue_alpha.xml"));
	}
	additonalInfo = -1;
}


void ComplexBucketData::createBox(float bucketSize) {
	box = Game::getScene()->CreateChild();
	box->SetPosition(Vector3(center.x_, 8, center.y_));
	box->Scale(Vector3(bucketSize * 0.8, 1, bucketSize * 0.8));
	box->Translate(Vector3::UP * 10, TS_PARENT);
	auto model = box->CreateComponent<StaticModel>();
	model->SetModel(Game::getCache()->GetResource<Model>("Models/plane.mdl"));
	model->SetMaterial(Game::getCache()->GetResource<Material>("Materials/blue_alpha.xml"));
	box->SetEnabled(true);
}

void ComplexBucketData::setCenter(float _centerX, float _centerY) {
	center = Vector2(_centerX, _centerY);
}

void ComplexBucketData::setEscapeThrought(int val) {
	escapeBucketIndex = val;
}

Urho3D::Vector2* ComplexBucketData::
getDirectrionFrom(Urho3D::Vector3* position, ComplexBucketData& escapeBucket) {
	return new Vector2(escapeBucket.getCenter().x_ - position->x_, escapeBucket.getCenter().y_ - position->z_);
}

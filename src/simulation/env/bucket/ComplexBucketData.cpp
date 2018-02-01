#include "ComplexBucketData.h"
#include "Game.h"
#include "objects/Static.h"
#include "utils.h"
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <iostream>


ComplexBucketData::ComplexBucketData() {
	box = nullptr;
	model = nullptr;
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
		model->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/red_alpha.xml"));
	}
	if (_object->getType() == BUILDING) {
		additonalInfo = _object->getPlayer();
	} else {
		additonalInfo = _object->getDbID();
	}
}

void ComplexBucketData::removeStatic() {
	object = nullptr;
	type = ObjectType::UNIT;
	if (box) {
		model->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/blue_alpha.xml"));
	}
	additonalInfo = -1;
}


void ComplexBucketData::createBox(double bucketSize) {
	box = Game::get()->getScene()->CreateChild();
	box->SetPosition(Vector3(center.x_, 8, center.y_));
	box->Scale(Vector3(bucketSize * 0.8, 1, bucketSize * 0.8));
	box->Translate(Vector3::UP * 10, TS_PARENT);
	model = box->CreateComponent<StaticModel>();
	model->SetModel(Game::get()->getCache()->GetResource<Model>("Models/plane.mdl"));
	model->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/blue_alpha.xml"));
	box->SetEnabled(true);
}

Vector3* ComplexBucketData::getDirectrionFrom(Vector3* position) {
	return new Vector3(position->x_ - center.x_, 0, position->z_ - center.y_);
}

void ComplexBucketData::setCenter(double _centerX, double _centerY) {
	center = Vector2(_centerX, _centerY);
}

Urho3D::Vector2& ComplexBucketData::getCenter() {
	return center;
}

void ComplexBucketData::setNeightbours(std::vector<std::pair<int, float>>* _neightbours) {
	neighbours.clear();
	neighbours.insert(neighbours.end(), _neightbours->begin(), _neightbours->end());
}

std::vector<std::pair<int, float>>& ComplexBucketData::getNeightbours() {
	return neighbours;
}

std::vector<std::pair<int, float>>& ComplexBucketData::getOccupiedNeightbours() {
	return occupiedNeightbours;
}

char ComplexBucketData::getAdditonalInfo() {
	return additonalInfo;
}

bool ComplexBucketData::isUnit() {
	return type == UNIT;
}

void ComplexBucketData::setOccupiedNeightbours(std::vector<std::pair<int, float>>* _occupiedNeightbours) {
	occupiedNeightbours.clear();
	occupiedNeightbours.insert(occupiedNeightbours.end(), _occupiedNeightbours->begin(), _occupiedNeightbours->end());
}

void ComplexBucketData::setEscapeThrought(int val) {//TODO uzyc tego
	escapeBucketIndex = val;
}

Urho3D::Vector3* ComplexBucketData::
getDirectrionFrom(Urho3D::Vector3* position, ComplexBucketData& escapeBucket) {
	return new Vector3(escapeBucket.getCenter().x_ - position->x_, 0, escapeBucket.getCenter().y_ - position->z_);
}

int ComplexBucketData::getEscapeBucket() {
	return escapeBucketIndex;
}

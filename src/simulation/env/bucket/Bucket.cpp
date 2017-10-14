#include "Bucket.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>

Bucket::~Bucket() {
	delete content;
	box->Remove();
}

Bucket::Bucket() {
	content = new std::vector<Unit *>();
	content->reserve(DEFAULT_VECTOR_SIZE / 2);
	box = nullptr;
	removeStatic();
}

std::vector<Unit *>* Bucket::getContent() {
	return content;
}

void Bucket::add(Unit* entity) {
	content->push_back(entity);
}

void Bucket::remove(Unit* entity) {
	ptrdiff_t pos = std::find(content->begin(), content->end(), entity) - content->begin();
	if (pos < content->size()) {
		content->erase(content->begin() + pos);
	}
}

ObjectType Bucket::getType() {
	return type;
}

void Bucket::setStatic(Static* _object) {
	object = _object;
	type = object->getType();
	content->clear();
	if (box) {
		model->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/red_alpha.xml"));
	}
}

void Bucket::removeStatic() {
	object = nullptr;
	type = ObjectType::UNIT;
	if (box) {
		model->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/blue_alpha.xml"));
	}
}

void Bucket::setCenter(double _centerX, double _centerY) {
	centerX = _centerX;
	centerY = _centerY;
}

void Bucket::createBox(double bucketSize) {
	box = Game::get()->getScene()->CreateChild();
	box->SetPosition(Vector3(centerX, 0, centerY));
	box->Scale(Vector3(bucketSize * 0.8, 1, bucketSize * 0.8));
	box->Translate(Vector3::UP * 10, TS_PARENT);
	model = box->CreateComponent<StaticModel>();
	model->SetModel(Game::get()->getCache()->GetResource<Model>("Models/box.mdl"));
	model->SetMaterial(Game::get()->getCache()->GetResource<Material>("Materials/blue_alpha.xml"));
	box->SetEnabled(true);
}

Vector3* Bucket::getDirectrionFrom(Vector3* position) {
	return new Vector3(centerX - position->x_, 0, centerY - position->z_);
}

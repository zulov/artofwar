#include "Bucket.h"


Bucket::~Bucket() {
	delete content;
}

Bucket::Bucket() {
	content = new std::vector<Unit *>();
	content->reserve(DEFAULT_VECTOR_SIZE / 2);
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
}

void Bucket::removeStatic() {
	object = nullptr;
	type = ObjectType::UNIT;
}

void Bucket::setCenter(double _centerX, double _centerY) {
	centerX = _centerX;
	centerY = _centerY;
}

Vector3* Bucket::getDirectrionFrom(Vector3* position) {
	return new Vector3(centerX - position->x_, 0, centerY - position->z_);
}

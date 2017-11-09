#include "Bucket.h"


Bucket::~Bucket() {
	delete content;
	delete data;
}

Bucket::Bucket() {
	content = new std::vector<Unit *>();
	content->reserve(DEFAULT_VECTOR_SIZE / 2);
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

void Bucket::setCenter(double _centerX, double _centerY) {
	data->setCenter(_centerX, _centerY);
}

Vector2& Bucket::getCenter() {
	return data->getCenter();
}

ObjectType Bucket::getType() {
	return data->getType();
}

void Bucket::setStatic(Static* object) {
	content->clear();
	data->setStatic(object);
}

void Bucket::removeStatic() {
	data->removeStatic();
}

void Bucket::createBox(double bucketSize) {
	data->createBox(bucketSize);
}

Vector3* Bucket::getDirectrionFrom(Vector3* position) {
	return data->getDirectrionFrom(position);
}

void Bucket::upgrade() {
	data = new ComplexBucketData();
}

void Bucket::setNeightbours(std::vector<std::pair<int, float>*>* tempNeighbour) {
	data->setNeightbours(tempNeighbour);
}

std::vector<std::pair<int, float>*>& Bucket::getNeightbours() {
	return data->getNeightbours();
}

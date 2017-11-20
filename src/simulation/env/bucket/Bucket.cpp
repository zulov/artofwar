#include "Bucket.h"
#include <iostream>


Bucket::~Bucket() {
	delete data;
}

Bucket::Bucket() {
	content.reserve(DEFAULT_VECTOR_SIZE / 2);
	size = 0;
	std::fill_n(unitsNumberPerPlayer, MAX_PLAYERS, 0);
}

std::vector<Unit *>& Bucket::getContent() {
	return content;
}

void Bucket::add(Unit* entity) {
	content.push_back(entity);
	++size;
	++unitsNumberPerPlayer[entity->getPlayer()];
}

void Bucket::remove(Unit* entity) {
	ptrdiff_t pos = std::find(content.begin(), content.end(), entity) - content.begin();
	//	if (pos < content->size()) {
	//		std::iter_swap(content->begin() + pos, content->end()-1);
	//		content->erase(content->end()-1);
	//		--size;
	//	}

	if (pos < content.size()) {
		content.erase(content.begin() + pos);
		--size;
		--unitsNumberPerPlayer[entity->getPlayer()];
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
	content.clear();
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

void Bucket::upgrade(ComplexBucketData* _data) {
	data = _data;
}

void Bucket::setNeightbours(std::vector<std::pair<int, float>*>* tempNeighbour) {
	data->setNeightbours(tempNeighbour);
}

std::vector<std::pair<int, float>*>& Bucket::getNeightbours() {
	return data->getNeightbours();
}

int& Bucket::getSize() {
	return size;
}

bool Bucket::incUnitsPerPlayer(content_info* ci) {
	if (size > 0) {
		for (int i = 0; i < MAX_PLAYERS; ++i) {
			ci->unitsNumberPerPlayer[i] += unitsNumberPerPlayer[i];
		}
		return true;
	}
	return false;
}

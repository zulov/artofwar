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

bool Bucket::incUnitsPerPlayer(content_info* ci, int activePlayer, bool checks[]) {
	bool hasUnits = false;
	if (size > 0) {
		for (int i = 0; i < MAX_PLAYERS; ++i) {
			if (((checks[3] && i == activePlayer) || (checks[4] && i != activePlayer)) && unitsNumberPerPlayer[i] > 0) {

				ci->unitsNumberPerPlayer[i] += unitsNumberPerPlayer[i];
				hasUnits = true;
			}
		}
		return hasUnits;
	}
	return false;
}

void Bucket::update(content_info* ci, bool checks[], int activePlayer) {
	switch (data->getType()) {
	case UNIT:
		{
		if (checks[3] || checks[4]) {
			const bool hasInc = incUnitsPerPlayer(ci, activePlayer, checks);
			if (hasInc) {
				ci->hasUnit = true;
			}
		}
		}
		break;
	case RESOURCE:
		if (checks[1]) {
			ci->hasResource = true;
			ci->resourceNumber[data->getAdditonalInfo()]++;
		}
		break;
	case BUILDING:
		if (checks[2]) {
			ci->hasBuilding = true;
			ci->buildingNumberPerPlayer[data->getAdditonalInfo()]++;
		}
		break;
	default: ;
	}
}

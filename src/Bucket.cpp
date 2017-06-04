#include "Bucket.h"


Bucket::~Bucket() {
	delete content;
}

Bucket::Bucket() {
	content = new std::list<Entity *>();
}

std::list<Entity *>* Bucket::getContent() {
	return content;
}

void Bucket::clearContent() {
	content->clear();
}

void Bucket::add(Entity* entity) {
	content->push_back(entity);
}

void Bucket::remove(Entity* entity) {
	content->remove(entity);
}

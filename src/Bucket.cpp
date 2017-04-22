#include "Bucket.h"


Bucket::~Bucket() {

}

Bucket::Bucket() {
	content = new std::vector<Entity *>();
	content->reserve(10);
}

std::vector<Entity *>* Bucket::getContent() {
	return content;
}

void Bucket::clearContent() {
	content->clear();
}

void Bucket::add(Entity* entity) {
	content->push_back(entity);
}

void Bucket::remove(Entity* entity) {
	ptrdiff_t pos = std::find(content->begin(), content->end(), entity) - content->begin();
	if (pos < content->size()) {
		content->erase(content->begin() + pos);
	}
}

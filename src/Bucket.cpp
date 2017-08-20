#include "Bucket.h"


Bucket::~Bucket() {
	delete content;
}

Bucket::Bucket() {
	content = new std::vector<Physical *>();
	content->reserve(DEFAULT_VECTOR_SIZE);
}

std::vector<Physical *>* Bucket::getContent() {
	return content;
}

void Bucket::clearContent() {
	content->clear();
}

void Bucket::add(Physical* entity) {
	content->push_back(entity);
}

void Bucket::remove(Physical* entity) {
	ptrdiff_t pos = std::find(content->begin(), content->end(), entity) - content->begin();
	if (pos < content->size()) {
		content->erase(content->begin() + pos);
	}
}

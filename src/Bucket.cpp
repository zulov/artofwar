#include "Bucket.h"


Bucket::~Bucket() {}

Bucket::Bucket() {
	sum = 0;
}

std::vector<Unit *> Bucket::getContent() {
	return content;
}

void Bucket::clearContent() {
	content.clear();
}

int Bucket::getSum() {
	return sum;
}

void Bucket::addToSum() {
	sum += content.size();
}

void Bucket::clearSum() {
	sum = 0;
}

void Bucket::add(Unit * entity) {
	content.push_back(entity);
}

void Bucket::remove(Unit * entity) {
	ptrdiff_t pos = std::find(content.begin(), content.end(), entity) - content.begin();
	if (pos < content.size()) {
		content.erase(content.begin()+pos);
	}
	
}

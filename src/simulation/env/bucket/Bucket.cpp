#include "Bucket.h"
#include "objects/Physical.h"


Bucket::Bucket() = default;

Bucket::~Bucket() = default;

void Bucket::add(Physical* entity) {
	content.push_back(entity);
}

void Bucket::remove(Physical* entity) {
	auto pos = std::find(content.begin(), content.end(), entity) - content.begin();

	if (pos < content.size()) {
		content.erase(content.begin() + pos);
		// std::iter_swap(content.begin() + pos, content.end()-1);
		// content.erase(content.end()-1);
	}
}

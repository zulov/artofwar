#include "Bucket.h"

#include "objects/Physical.h"

int Bucket::getSize() const {
	return content.size();
}

void Bucket::reserve(int i) {
	content.reserve(i);
}

void Bucket::add(Physical* entity) {
	content.push_back(entity);
}

void Bucket::add(const std::vector<Physical*>& things) {
	content.insert(content.end(), things.begin(), things.end());
}

void Bucket::remove(Physical* entity) {
	auto it = std::ranges::find(content, entity);

	if (it != content.end()) {
		std::iter_swap(it, content.end() - 1);
		content.pop_back();
	} else {
		assert(false);
	}
}

void Bucket::clear() {
	content.clear();
}

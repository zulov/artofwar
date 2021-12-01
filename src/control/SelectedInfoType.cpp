#include "SelectedInfoType.h"
#include "objects/Physical.h"


SelectedInfoType::SelectedInfoType(unsigned short id):id(id) {
	data.reserve(10);
}

void SelectedInfoType::clear() {
	data.clear();
	std::fill_n(levels.begin(), levels.size(), false);
}

void SelectedInfoType::add(Physical* physical) {
	assert(physical->getLevelNum() <= (int)levels.size());
	assert(physical->getId() == id);
	data.push_back(physical);
	if (physical->getLevelNum() >= 0) {
		levels[physical->getLevelNum()] = true;
	}
}

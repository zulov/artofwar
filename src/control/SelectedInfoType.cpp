#include "SelectedInfoType.h"
#include "objects/Physical.h"


SelectedInfoType::SelectedInfoType() {
	data.reserve(100);
	id = -1;
}

void SelectedInfoType::clear() {
	data.clear();
	levels.clear();
	id = -1;
}

void SelectedInfoType::add(Physical* physical) {
	data.push_back(physical);
	levels.insert(physical->getLevelNum());
	id = physical->getId();
}

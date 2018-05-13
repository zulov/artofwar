#include "SelectedInfoType.h"


SelectedInfoType::SelectedInfoType() {
	data.reserve(100);
	id = -1;
}


SelectedInfoType::~SelectedInfoType() = default;

void SelectedInfoType::clear() {
	data.clear();
	id = -1;
}

void SelectedInfoType::add(Physical* physical) {
	data.push_back(physical);
	id = physical->getDbID();
}


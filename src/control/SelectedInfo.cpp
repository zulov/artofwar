#include "SelectedInfo.h"
#include "Controls.h"
#include "ObjectEnums.h"
#include "SelectedInfoType.h"
#include "objects/Physical.h"
#include "utils.h"


SelectedInfo::SelectedInfo() {
	allNumber = 0;
	allSubTypeNumber = 0;
	selectedByType.reserve(MAX_SIZE_TYPES);

	for (int i = 0; i < MAX_SIZE_TYPES; ++i) {
		selectedByType.push_back(new SelectedInfoType());
	}
}

SelectedInfo::~SelectedInfo() {
	clear_vector(selectedByType);
}

void SelectedInfo::setAllNumber(int allNumber) {
	this->allNumber = allNumber;
	changed = true;
}

void SelectedInfo::setSelectedType(ObjectType selectedType) {
	this->selectedType = selectedType;
	changed = true;
}

void SelectedInfo::reset() {
	changed = true;
	allNumber = 0;
	allSubTypeNumber = 0;
	for (auto& element : selectedByType) {
		element->clear();
	}
}

void SelectedInfo::select(Physical* entity) {
	if (entity->getDbID() >= 0) {
		selectedByType.at(entity->getDbID())->add(entity);
		if (selectedByType.at(entity->getDbID())->getData().size() == 1) {
			++allSubTypeNumber;
		}
	}
	changed = true;
}

void SelectedInfo::hasBeenUpdatedDrawn() {
	changed = false;
}

std::optional<SelectedInfoType*> SelectedInfo::getOneSelectedTypeInfo() const {
	for (auto info : selectedByType) {
		if (!info->getData().empty()) {
			return info;
		}
	}
	return {};
}

bool SelectedInfo::isSthSelected() const {
	return allNumber > 0 && selectedType != ObjectType::PHYSICAL;
}

void SelectedInfo::refresh(std::vector<Physical*>* selected) {
	reset();
	for (auto physical : *selected) {
		select(physical);
	}
}

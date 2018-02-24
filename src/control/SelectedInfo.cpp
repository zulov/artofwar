#include "SelectedInfo.h"
#include "Controls.h"
#include "SelectedInfoType.h"
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

bool SelectedInfo::hasChanged() {
	return changed;
}

void SelectedInfo::setAllNumber(int allNumber) {
	this->allNumber = allNumber;
	changed = true;
}

void SelectedInfo::setSelectedType(ObjectType selectedType) {
	this->selectedType = selectedType;
	changed = true;
}

ObjectType SelectedInfo::getSelectedType() {
	return selectedType;
}

int SelectedInfo::getAllNumber() {
	return allNumber;
}

void SelectedInfo::reset() {
	changed = true;
	allNumber = 0;
	allSubTypeNumber = 0;
	for (auto & element : selectedByType) {
		element->clear();
	}
}

vector<SelectedInfoType*>& SelectedInfo::getSelectedTypes() {
	return selectedByType;
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

int SelectedInfo::getSelectedSubTypeNumber() {
	return allSubTypeNumber;
}

void SelectedInfo::hasBeedUpdatedDrawn() {
	changed = false;
}

String& SelectedInfo::getMessage() {
	return message;
}

void SelectedInfo::setMessage(String& s) {
	message = String(s);
	changed = true;
}

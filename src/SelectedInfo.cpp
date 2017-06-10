#include "SelectedInfo.h"


SelectedInfo::SelectedInfo() {
	message = new Urho3D::String();
	allNumber = 0;
}


SelectedInfo::~SelectedInfo() {
}

Urho3D::String* SelectedInfo::getMessage() {
	delete message;
	message = new Urho3D::String("Zaznaczeni: " + Urho3D::String(allNumber));
	return message;
	changed = false;
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

void SelectedInfo::setUnitsTypes(UnitType unitTypes[]) {
	for (int i = 0; i < SELECTED_INFO_SIZE; ++i) {
		this->unitTypes[i] = unitTypes[i];
	}
	changed = true;
}

void SelectedInfo::setBuildingTypes(BuildingType buildingTypes[]) {
	for (int i = 0; i < SELECTED_INFO_SIZE; ++i) {
		this->buildingTypes[i] = buildingTypes[i];
	}
	changed = true;
}

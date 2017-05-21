#include "HudElement.h"


HudElement::HudElement(Urho3D::UIElement* _uiElement) {
	uiElement = _uiElement;
}

HudElement::~HudElement() {
}

Urho3D::UIElement* HudElement::getUIElement() {
	return uiElement;
}

UnitType HudElement::getUnitType() const {
	return unitType;
}

void HudElement::setUnitType(UnitType unitType) {
	this->unitType = unitType;
}

BuildingType HudElement::getBuildingType() const {
	return buildingType;
}

void HudElement::setBuildingType(BuildingType buildingType) {
	this->buildingType = buildingType;
}

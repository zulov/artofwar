#include "HudElement.h"


HudElement::HudElement(Urho3D::UIElement* _uiElement) {
	uiElement = _uiElement;
}

HudElement::~HudElement() = default;

Urho3D::UIElement* HudElement::getUIElement() {
	return uiElement;
}

void HudElement::setId(short id, ObjectType _type) {
	objectId = id;
	type = _type;
}

void HudElement::setSubType(short _subType) {
	subType = _subType;
}

short HudElement::getId() {
	return objectId;
}

short HudElement::getSubType() {
	return subType;
}

ObjectType HudElement::getType() {
	return type;
}

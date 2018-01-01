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

short HudElement::getId() {
	return objectId;
}

ObjectType HudElement::getType() {
	return type;
}

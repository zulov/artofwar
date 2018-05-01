#include "HudData.h"


HudData::HudData(Urho3D::UIElement* _uiParent) :parent(_uiParent){
}

HudData::~HudData() = default;

Urho3D::UIElement* HudData::getUIParent() {
	return parent;
}

void HudData::setId(short _id) {
	id = _id;
}

void HudData::setId(short _id, LeftMenuAction _type) {
	id = _id;
	type = _type;
}

short HudData::getId() {
	return id;
}

LeftMenuAction HudData::getType() {
	return type;
}


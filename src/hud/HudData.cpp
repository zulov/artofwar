#include "HudData.h"


HudData::HudData(Urho3D::UIElement* _uiParent) :parent(_uiParent){
}

HudData::~HudData() = default;

void HudData::setId(short _id) {
	id = _id;
}

void HudData::setId(short _id, MenuAction _type) {
	id = _id;
	type = _type;
}

MenuAction HudData::getType() const {
	return type;
}


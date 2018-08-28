#include "HudData.h"


HudData::HudData(Urho3D::UIElement* _uiParent) : parent(_uiParent) {
}

HudData::~HudData() = default;

void HudData::set(short _id, MenuAction _type, Urho3D::String _text) {
	id = _id;
	type = _type;
	text = _text;
}

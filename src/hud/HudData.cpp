#include "HudData.h"

#include <Urho3D/UI/UIEvents.h>
#include <utility>

HudData::HudData(Urho3D::UIElement* _uiParent) : parent(_uiParent) {
}

void HudData::set(short _id, ActionType _type, Urho3D::String _text) {
	id = _id;
	type = _type;
	text = std::move(_text);
}

HudData* HudData::getFromElement(Urho3D::VariantMap& eventData) {
	const auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	return static_cast<HudData *>(element->GetVar("HudElement").GetVoidPtr());
}


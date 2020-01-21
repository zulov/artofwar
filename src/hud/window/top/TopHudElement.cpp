#include "TopHudElement.h"
#include "../../UiUtils.h"
#include <Urho3D/UI/ToolTip.h>

TopHudElement::TopHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style, Urho3D::Texture2D* texture, Urho3D::String styleName) {
	button = createElement<Urho3D::Button>(parent, style, styleName );
	icon = createSprite(button, texture, style, "SpriteLeft");

	mock = createElement<Urho3D::UIElement>(button,  style, "mockCenter");

	value = addChildText(mock, "TopText", style);

	button->SetVar("TopHudElement", this);
}


TopHudElement::~TopHudElement() = default;

void TopHudElement::hide() const {
	button->SetVisible(false);
}

void TopHudElement::show() const {
	button->SetVisible(true);
}

void TopHudElement::setText(const Urho3D::String& msg) const {
	value->SetText(msg);
}

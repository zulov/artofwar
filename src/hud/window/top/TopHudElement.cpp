#include "TopHudElement.h"
#include "../../UiUtils.h"
#include <Urho3D/UI/ToolTip.h>

TopHudElement::TopHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style, Urho3D::Texture2D* texture,
                             Urho3D::String styleName) {
	button = createElement<Urho3D::Button>(parent, style, styleName);
	icon = createSprite(button, texture, style, "SpriteLeft");

	const auto mock = createElement<Urho3D::UIElement>(button, style, "mockCenter");

	value = addChildText(mock, "TopText", style);
	value1 = addChildText(mock, "TopTextSmall", style);

	button->SetVar("TopHudElement", this);
}

void TopHudElement::hide() const {
	button->SetVisible(false);
}

void TopHudElement::show() const {
	button->SetVisible(true);
}

void TopHudElement::setText(const Urho3D::String& msg, const Urho3D::String& msg1) const {
	value->SetText(msg);
	value1->SetText(msg1);
}

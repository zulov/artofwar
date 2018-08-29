#include "TopHudElement.h"
#include "../../UiUtils.h"

TopHudElement::TopHudElement(Urho3D::XMLFile* style, Urho3D::Texture2D* texture) {
	icon = createSprite(texture, style, "SpriteLeft");
	button = simpleButton(icon, style, "TopButtons");

	mock = button->CreateChild<Urho3D::UIElement>();
	mock->SetStyle("mockCenter", style);

	value = addChildText(mock, "TopText", "", style);

	button->SetVar("TopHudElement", this);
}


TopHudElement::~TopHudElement() = default;

Urho3D::Button* TopHudElement::getButton() {
	return button;
}

void TopHudElement::hide() {
	button->SetVisible(false);
}

void TopHudElement::show() {
	button->SetVisible(true);
}

void TopHudElement::setText(const Urho3D::String& msg) {
	value->SetText(msg);
}

#include "TopHudElement.h"
#include "../../UiUtils.h"

TopHudElement::TopHudElement(Texture2D* texture) {
	icon = createSprite(texture, "SpriteLeft");
	button = simpleButton(icon, "TopButtons");

	mock = button->CreateChild<UIElement>();
	mock->SetStyle("mockCenter");

	value = mock->CreateChild<Text>();
	value->SetStyle("TopText");

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

#include "TopHudElement.h"
#include "../ButtonUtils.h"

TopHudElement::TopHudElement(Urho3D::XMLFile* style, Texture2D* texture) {
	icon = createSprite(texture, style, "SpriteLeft");
	button = simpleButton(icon, style, "TopButtons");

	mock = button->CreateChild<UIElement>();
	mock->SetStyle("mockCenter", style);

	value = mock->CreateChild<Text>();
	value->SetStyle("MiniText", style);

	button->AddChild(icon);
	button->SetVar("TopHudElement", this);
}


TopHudElement::~TopHudElement() {
}

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

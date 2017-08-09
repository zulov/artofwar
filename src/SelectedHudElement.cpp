#include "SelectedHudElement.h"
#include <Urho3D/UI/Text.h>
#include "ButtonUtils.h"


SelectedHudElement::SelectedHudElement(Urho3D::XMLFile* _style) {
	selected = new Physical*[MAX_SELECTED_IN_BUTTON];
	for (int i = 0; i < MAX_SELECTED_IN_BUTTON; ++i) {
		selected[i] = nullptr;
	}
	style = _style;
	button = simpleButton(nullptr, style, "SmallIcon");
	button->SetVisible(false);
	text = button->CreateChild<Text>();
	text->SetStyle("MyText", style);
	icon = createEmptySprite(style, "SmallSprite");
	button->AddChild(icon);
}


SelectedHudElement::~SelectedHudElement() {
}

void SelectedHudElement::unSelect() {
}

Button* SelectedHudElement::getButton() {
	return button;
}

void SelectedHudElement::hide() {
	button->SetVisible(false);
}

void SelectedHudElement::show() {
	button->SetVisible(true);

}

void SelectedHudElement::setText(const String& msg) {
	text->SetVisible(true);
	text->SetText(msg);
}

void SelectedHudElement::hideText() {
	text->SetVisible(false);
}

void SelectedHudElement::setTexture(Texture2D* texture) {
	setTextureToSprite(icon, texture);
}

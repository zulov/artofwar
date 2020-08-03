#include "SelectedHudElement.h"
#include <Urho3D/UI/Text.h>
#include "hud/UiUtils.h"
#include "objects/Physical.h"


SelectedHudElement::SelectedHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style) {
	selected.reserve(MAX_SELECTED_IN_BUTTON);

	button = createElement<Urho3D::Button>(parent, style, "Icon");
	button->SetVisible(false);
	text = addChildText(button, "MyText", style);

	icon = createElement<MySprite>(button, style, "NormalSprite");
	button->SetVar("SelectedHudElement", this);
	mock = createElement<Urho3D::UIElement>(button, style, "mock");

	bars = createElement<Urho3D::ProgressBar>(mock, style, "MiniProgressBar");
	bars->SetRange(1);
	bars->SetValue(0.5);
	bars->SetVisible(false);
}

Urho3D::Button* SelectedHudElement::getButton() const {
	return button;
}

void SelectedHudElement::hide() const {
	button->SetVisible(false);
}

void SelectedHudElement::show() const {
	button->SetVisible(true);
}

void SelectedHudElement::setText(const Urho3D::String& msg) const {
	text->SetVisible(true);
	text->SetText(msg);
}

void SelectedHudElement::hideText() const {
	text->SetVisible(false);
}

void SelectedHudElement::setTexture(Urho3D::Texture2D* texture) const {
	setTextureToSprite(icon, texture);
}

void SelectedHudElement::add(std::vector<Physical*>& physicals, int start, int end) {
	selected.clear();

	float sum = 0;
	for (int i = start; i < end; ++i) {
		selected.push_back(physicals.at(i));
		int ind = i - start;
		if (ind < MAX_SELECTED_IN_BUTTON) {
			sum += physicals.at(i)->getHealthPercent();
		}
	}

	bars->SetRange(1);
	bars->SetValue(sum + (end - start));
	bars->SetVisible(true);
}

std::vector<Physical*>& SelectedHudElement::getSelected() {
	return selected;
}

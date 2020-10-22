#include "SelectedHudElement.h"

#include <Urho3D/UI/ProgressBar.h>
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
	const auto mock = createElement<Urho3D::UIElement>(button, style, "mock");

	bar = createElement<Urho3D::ProgressBar>(mock, style, "MiniProgressBar");
	bar->SetRange(1);
	bar->SetValue(0.5);
	bar->SetVisible(false);
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

	bar->SetRange(1);
	bar->SetValue(sum + (end - start));
	bar->SetVisible(true);
}

std::vector<Physical*>& SelectedHudElement::getSelected() {
	return selected;
}

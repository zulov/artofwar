#include "SelectedHudElement.h"
#include "hud/UiUtils.h"
#include "objects/Physical.h"
#include <Urho3D/UI/Text.h>


SelectedHudElement::SelectedHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style) {
	selected = new std::vector<Physical*>();
	selected->reserve(MAX_SELECTED_IN_BUTTON);

	button = createElement<Urho3D::Button>(parent,style, "SmallIcon");
	button->SetVisible(false);
	text = addChildText(button, "MyText", style);

	icon = createElement<MySprite>(button, style, "SmallSprite");
	button->SetVar("SelectedHudElement", this);
	mock = createElement<Urho3D::UIElement>(button, style, "mock");

	bars = new Urho3D::ProgressBar*[MAX_SELECTED_IN_BUTTON];
	for (int i = 0; i < MAX_SELECTED_IN_BUTTON; ++i) {
		bars[i] = createElement<Urho3D::ProgressBar>(mock, style, "MiniProgressBar");
		bars[i]->SetRange(1);
		bars[i]->SetValue(0.5);
		bars[i]->SetVisible(false);
	}
}


SelectedHudElement::~SelectedHudElement() {
	delete selected;
	delete[] bars;
}

Urho3D::Button* SelectedHudElement::getButton() {
	return button;
}

void SelectedHudElement::hide() {
	button->SetVisible(false);
}

void SelectedHudElement::show() {
	button->SetVisible(true);
}

void SelectedHudElement::setText(const Urho3D::String& msg) {
	text->SetVisible(true);
	text->SetText(msg);
}

void SelectedHudElement::hideText() {
	text->SetVisible(false);
}

void SelectedHudElement::setTexture(Urho3D::Texture2D* texture) {
	setTextureToSprite(icon, texture);
}

void SelectedHudElement::add(std::vector<Physical*>& physicals, int start, int end) {
	selected->clear();

	for (int i = 0; i < MAX_SELECTED_IN_BUTTON; ++i) {
		bars[i]->SetVisible(false);
	}
	for (int i = start; i < end; ++i) {
		selected->push_back(physicals.at(i));
		int ind = i - start;
		if (ind < MAX_SELECTED_IN_BUTTON) {
			bars[ind]->SetRange(1);
			bars[ind]->SetValue(physicals.at(i)->getHealthPercent());
			bars[ind]->SetVisible(true);
		}
	}
}

std::vector<Physical*>* SelectedHudElement::getSelected() {
	return selected;
}

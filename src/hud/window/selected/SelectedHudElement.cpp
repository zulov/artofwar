#include "SelectedHudElement.h"
#include "hud/UiUtils.h"
#include <Urho3D/UI/Text.h>



SelectedHudElement::SelectedHudElement(Urho3D::XMLFile* style) {
	selected = new std::vector<Physical*>();
	selected->reserve(MAX_SELECTED_IN_BUTTON);

	button = simpleButton(nullptr, style, "SmallIcon");
	button->SetVisible(false);
	text = button->CreateChild<Text>();
	text->SetStyle("MyText", style);
	icon = createEmptySprite(style, "SmallSprite");
	button->AddChild(icon);
	button->SetVar("SelectedHudElement", this);
	mock = button->CreateChild<UIElement>();
	mock->SetStyle("mock", style);

	bars = new ProgressBar*[MAX_SELECTED_IN_BUTTON];
	for (int i = 0; i < MAX_SELECTED_IN_BUTTON; ++i) {
		bars[i] = mock->CreateChild<ProgressBar>();
		bars[i]->SetStyle("MiniProgressBar");
		bars[i]->SetRange(1);
		bars[i]->SetValue(0.5);
		bars[i]->SetVisible(false);
	}
}


SelectedHudElement::~SelectedHudElement() {
	delete selected;
	delete[] bars;
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

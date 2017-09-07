#include "SelectedHudElement.h"
#include <Urho3D/UI/Text.h>
#include "hud/ButtonUtils.h"


SelectedHudElement::SelectedHudElement(Urho3D::XMLFile* _style) {
	selected = new std::vector<Physical*>();
	selected->reserve(MAX_SELECTED_IN_BUTTON);

	style = _style;
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
		bars[i]->SetStyle("MiniProgressBar", style);
		bars[i]->SetRange(1);
		bars[i]->SetValue(0.5);
		bars[i]->SetVisible(false);
	}
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

void SelectedHudElement::add(vector<Physical*>* physicals) {
	selected->clear();

	for (int i = 0; i < MAX_SELECTED_IN_BUTTON; ++i) {
		bars[i]->SetVisible(false);
	}
	for (int i = 0; i < physicals->size(); ++i) {
		selected->push_back(physicals->at(i));
		if (i < MAX_SELECTED_IN_BUTTON) {
			bars[i]->SetRange(1);
			bars[i]->SetValue(physicals->at(i)->getHealthBarSize());
			bars[i]->SetVisible(true);
		}
	}
}

std::vector<Physical*>* SelectedHudElement::getSelected() {
	return selected;
}

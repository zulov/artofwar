#include "QueueHudElement.h"
#include "../ButtonUtils.h"


QueueHudElement::QueueHudElement(Urho3D::XMLFile* style) {
	button = simpleButton(nullptr, style, "SmallIcon");
	button->SetVisible(false);
	text = button->CreateChild<Text>();
	text->SetStyle("MiniText", style);
	icon = createEmptySprite(style, "SmallSprite");
	button->AddChild(icon);
	button->SetVar("SelectedHudElement", this);
	mock = button->CreateChild<UIElement>();
	mock->SetStyle("mock", style);

	bar = mock->CreateChild<ProgressBar>();
	bar->SetStyle("MiniProgressBar", style);
	bar->SetRange(1);
	bar->SetValue(0.5);
	bar->SetVisible(true);	
}


QueueHudElement::~QueueHudElement() {
}

Urho3D::Button* QueueHudElement::getButton() {
	return button;
}

void QueueHudElement::hide() {
	button->SetVisible(false);
}

void QueueHudElement::show() {
	button->SetVisible(true);
}

void QueueHudElement::setText(const String& msg) {
	text->SetVisible(true);
	text->SetText(msg);
}

void QueueHudElement::hideText() {
	text->SetVisible(false);
}

void QueueHudElement::setTexture(Texture2D* texture) {
	setTextureToSprite(icon, texture);
}

void QueueHudElement::setProgress(float progress) {
	bar->SetValue(progress);
}

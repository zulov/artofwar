#include "QueueHudElement.h"
#include <Urho3D/UI/ProgressBar.h>
#include "../../UiUtils.h"
#include "objects/queue/QueueElement.h"


QueueHudElement::QueueHudElement(Urho3D::XMLFile* style) {
	button = simpleButton(nullptr, style, "SmallIcon");
	button->SetVisible(false);

	text = addChildText(button, "MiniText", "", style);

	icon = createEmptySprite(style, "SmallSprite");
	button->AddChild(icon);
	button->SetVar("QueueHudElement", this);
	mock = createElement<Urho3D::UIElement>(button, style, "mock");

	bar = createElement<Urho3D::ProgressBar>(mock, style, "QueueProgressBar");
	bar->SetRange(1);
	bar->SetValue(0.5);
	bar->SetVisible(true);

	element = nullptr;
}


QueueHudElement::~QueueHudElement() = default;

Urho3D::Button* QueueHudElement::getButton() {
	return button;
}

void QueueHudElement::hide() {
	if (button->IsVisible()) {
		button->SetVisible(false);
	}
}

void QueueHudElement::show() {
	if (!button->IsVisible()) {
		button->SetVisible(true);
	}
}

void QueueHudElement::setText(const Urho3D::String& msg) {
	text->SetVisible(true);
	text->SetText(msg);
}

void QueueHudElement::hideText() {
	text->SetVisible(false);
}

void QueueHudElement::setTexture(Urho3D::Texture2D* texture) {
	setTextureToSprite(icon, texture);
}

void QueueHudElement::setProgress(float progress) {
	bar->SetValue(progress);
}

void QueueHudElement::setData(QueueElement* _element) {
	element = _element;
}

void QueueHudElement::reduce(short amount) {
	element->reduce(amount);
}

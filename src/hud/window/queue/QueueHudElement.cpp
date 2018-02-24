#include "QueueHudElement.h"
#include "../../UiUtils.h"
#include "objects/queue/QueueElement.h"


QueueHudElement::QueueHudElement(Urho3D::XMLFile* style) {
	button = simpleButton(nullptr, style, "SmallIcon");
	button->SetVisible(false);

	text = button->CreateChild<Text>();
	text->SetStyle("MiniText", style);

	icon = createEmptySprite(style, "SmallSprite");
	button->AddChild(icon);
	button->SetVar("QueueHudElement", this);
	mock = button->CreateChild<UIElement>();
	mock->SetStyle("mock", style);

	bar = mock->CreateChild<ProgressBar>();
	bar->SetStyle("MiniProgressBar", style);
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

void QueueHudElement::setData(QueueElement* _element) {
	element = _element;
}

void QueueHudElement::reduce(short amount) {
	element->reduce(amount);
}

#include "QueueHudElement.h"
#include "../../UiUtils.h"
#include "objects/queue/QueueElement.h"
#include <Urho3D/UI/ProgressBar.h>
#include <Urho3D/UI/UIEvents.h>


QueueHudElement::QueueHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style) {
	button = createElement<Urho3D::Button>(parent, style, "SmallIcon");
	button->SetVisible(false);

	text = addChildText(button, "MiniText", style);
	icon = createElement<MySprite>(button, style, "SmallSprite");

	button->SetVar("QueueHudElement", this);
	const auto mock = createElement<Urho3D::UIElement>(button, style, "mock");

	bar = createElement<Urho3D::ProgressBar>(mock, style, "QueueProgressBar");
	bar->SetRange(1);
	bar->SetValue(0.5);
	bar->SetVisible(true);

	element = nullptr;
}

Urho3D::Button* QueueHudElement::getButton() const {
	return button;
}

void QueueHudElement::hide() const {
	if (button->IsVisible()) {
		button->SetVisible(false);
	}
}

void QueueHudElement::show() const {
	if (!button->IsVisible()) {
		button->SetVisible(true);
	}
}

void QueueHudElement::setText(const Urho3D::String& msg) const {
	text->SetVisible(true);
	text->SetText(msg);
}

void QueueHudElement::hideText() const {
	text->SetVisible(false);
}

void QueueHudElement::setTexture(Urho3D::Texture2D* texture) const {
	setTextureToSprite(icon, texture);
}

void QueueHudElement::setProgress(float progress) const {
	bar->SetValue(progress);
}

void QueueHudElement::setData(QueueElement* _element) {
	element = _element;
}

void QueueHudElement::reduce(short amount) const {
	element->reduce(amount);
}

QueueHudElement* QueueHudElement::getFromElement(Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	return static_cast<QueueHudElement *>(element->GetVar("QueueHudElement").GetVoidPtr());
}

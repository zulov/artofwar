#include "TopHudElement.h"
#include "../../UiUtils.h"
#include <Urho3D/UI/ToolTip.h>

TopHudElement::TopHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style, Urho3D::Texture2D* texture,
                             bool toolTipEnabled, Urho3D::String styleName) {
	button = createElement<Urho3D::Button>(parent, style, styleName);
	icon = createSprite(button, texture, style, "SpriteLeft");

	const auto mock = createElement<Urho3D::UIElement>(button, style, "mockCenter");

	value = addChildText(mock, "TopText", style);
	value1 = addChildText(mock, "TopTextSmall", style);

	button->SetVar("TopHudElement", this);

	if (toolTipEnabled) {
		toolTip = createElement<Urho3D::ToolTip>(button, style, "TopToolTip");

		auto* textHolder = createElement<Urho3D::BorderImage>(toolTip, style, "ToolTipBorderImage");

		tooltipText = createElement<Urho3D::Text>(textHolder, style, "ToolTipText");
	}
}

void TopHudElement::hide() const {
	button->SetVisible(false);
}

void TopHudElement::show() const {
	button->SetVisible(true);
}

const TopHudElement* TopHudElement::setText(const Urho3D::String& msg, const Urho3D::String& msg1) const {
	value->SetText(msg);
	value1->SetText("(" + msg1 + ")");
	return this;
}

void TopHudElement::setToolTip(const Urho3D::String& msg) const {
	tooltipText->SetText(msg);
}

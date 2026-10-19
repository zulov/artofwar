#pragma once
#include "Urho3D/Container/Str.h"

namespace Urho3D {
	class Color;
	class String;
	class Button;
	class Text;
	class Texture2D;
	class ToolTip;
	class UIElement;
	class XMLFile;
}

class MySprite;

class TopHudElement {
public:
	TopHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style, Urho3D::Texture2D* texture,
	              bool toolTipEnabled = true, Urho3D::String styleName = "TopButtons");
	~TopHudElement() = default;
	void hide() const;
	void show() const;
	const TopHudElement* setText(const Urho3D::String& msg, const Urho3D::String& msg1) const;
	const TopHudElement* setRatioText(const Urho3D::String& numerator, const Urho3D::String& denominator) const;
	void setTextColor(const Urho3D::Color& primary, const Urho3D::Color& secondary) const;
	void setToolTip(const Urho3D::String& msg) const;

private:
	MySprite* icon;
	Urho3D::Button* button;
	Urho3D::Text* value;
	Urho3D::Text* value1;
	Urho3D::ToolTip* toolTip;
	Urho3D::Text* tooltipText;
};

#pragma once
#include "Urho3D/Container/Str.h"

namespace Urho3D {
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
	              Urho3D::String styleName = "TopButtons");
	~TopHudElement() = default;
	void hide() const;
	void show() const;
	void setText(const Urho3D::String& msg) const;
private:
	MySprite* icon;
	Urho3D::Button* button;
	Urho3D::Text* value;
	Urho3D::ToolTip* toolTip;
};

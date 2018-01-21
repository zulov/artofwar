#pragma once

#include "hud/MySprite.h"
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Button.h>

class TopHudElement
{
public:
	TopHudElement(Texture2D* texture);
	~TopHudElement();
	Urho3D::Button* getButton();
	void hide();
	void show();
	void setText(const Urho3D::String& msg);
private:
	Urho3D::Button* button;

	MySprite* icon;

	Urho3D::Text* value;
	Urho3D::UIElement* mock;
};

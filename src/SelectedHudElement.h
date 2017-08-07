#pragma once
#include <Urho3D/UI/Button.h>
#include "Physical.h"
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/Text.h>

#define MAX_SELECTED_IN_BUTTON 3

class SelectedHudElement
{
public:
	SelectedHudElement(Urho3D::XMLFile* _style);
	~SelectedHudElement();
	void unSelect();
	void select(ObjectType objetType, int subType);
	Button * getButton();
	void hide();
	void show();

	void setText(const String& msg);
	void hideText();
private:
	Urho3D::Button* button;
	Physical** selected;
	Urho3D::XMLFile* style;
	Sprite* icon;
	Text* text;
};

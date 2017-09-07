#pragma once
#include <Urho3D/UI/Button.h>
#include "objects/Physical.h"
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ProgressBar.h>
#include "hud/MySprite.h"

#define MAX_SELECTED_IN_BUTTON 2

class SelectedHudElement
{
public:
	SelectedHudElement(Urho3D::XMLFile* _style);
	~SelectedHudElement();
	void unSelect();
	void select(ObjectType objetType, int subType);
	Button* getButton();
	void hide();
	void show();

	void setText(const String& msg);
	void hideText();
	void setTexture(Texture2D* texture);
	void add(std::vector<Physical*> *physicals);
	std::vector<Physical*>* getSelected();
private:
	Urho3D::Button* button;
	std::vector<Physical*>* selected;
	Urho3D::XMLFile* style;
	MySprite* icon;
	ProgressBar** bars;
	Text* text;
	UIElement* mock;
};

#pragma once
#include "objects/Physical.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/ProgressBar.h>
#include <Urho3D/UI/Text.h>


#define MAX_SELECTED_IN_BUTTON 4

class MySprite;

class SelectedHudElement
{
public:
	SelectedHudElement(Urho3D::XMLFile* style);
	~SelectedHudElement();

	Button* getButton();
	void hide();
	void show();

	void setText(const String& msg);
	void hideText();
	void setTexture(Texture2D* texture);
	void add(std::vector<Physical*>& physicals, int start, int end);
	std::vector<Physical*>* getSelected();
private:
	Urho3D::Button* button;
	std::vector<Physical*>* selected;

	MySprite* icon;
	ProgressBar** bars;
	Text* text;
	UIElement* mock;
};

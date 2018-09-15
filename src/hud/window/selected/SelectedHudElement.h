#pragma once

#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/ProgressBar.h>
#include <Urho3D/UI/Text.h>

#define MAX_SELECTED_IN_BUTTON 4

class MySprite;
class Physical;

class SelectedHudElement
{
public:
	explicit SelectedHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style);
	~SelectedHudElement();

	Urho3D::Button* getButton();
	void hide();
	void show();

	void setText(const Urho3D::String& msg);
	void hideText();
	void setTexture(Urho3D::Texture2D* texture);
	void add(std::vector<Physical*>& physicals, int start, int end);
	std::vector<Physical*>* getSelected();
private:
	Urho3D::Button* button;
	std::vector<Physical*>* selected;

	MySprite* icon;
	Urho3D::ProgressBar** bars;
	Urho3D::Text* text;
	Urho3D::UIElement* mock;
};

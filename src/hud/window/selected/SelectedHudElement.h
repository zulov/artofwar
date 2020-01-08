#pragma once

#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/ProgressBar.h>
#include <Urho3D/UI/Text.h>

#define MAX_SELECTED_IN_BUTTON 5

class MySprite;
class Physical;

class SelectedHudElement
{
public:
	explicit SelectedHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style);
	~SelectedHudElement();

	Urho3D::Button* getButton() const;
	void hide() const;
	void show() const;

	void setText(const Urho3D::String& msg) const;
	void hideText() const;
	void setTexture(Urho3D::Texture2D* texture) const;
	void add(std::vector<Physical*>& physicals, int start, int end);
	std::vector<Physical*>& getSelected();
private:
	Urho3D::Button* button;
	std::vector<Physical*> selected;

	MySprite* icon;
	Urho3D::ProgressBar* bars;
	Urho3D::Text* text;
	Urho3D::UIElement* mock;
};

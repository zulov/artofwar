#pragma once
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/Text.h>
#include "MySprite.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/ProgressBar.h>

class QueueHudElement
{
public:
	QueueHudElement(Urho3D::XMLFile* _style);
	~QueueHudElement();
	Urho3D::Button* getButton();
	void hide();
	void show();

	void setText(const Urho3D::String& msg);
	void hideText();
	void setTexture(Urho3D::Texture2D* texture);
	void setProgress(float progress);
private:
	Urho3D::Button* button;
	Urho3D::XMLFile* style;
	MySprite* icon;
	ProgressBar* bar;
	Urho3D::Text* text;
	Urho3D::UIElement* mock;
};


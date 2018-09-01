#pragma once

#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/ProgressBar.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIElement.h>


class QueueElement;
class MySprite;

class QueueHudElement
{
public:
	explicit QueueHudElement(Urho3D::XMLFile* style);
	~QueueHudElement();
	Urho3D::Button* getButton();
	void hide();
	void show();

	void setText(const Urho3D::String& msg);
	void hideText();
	void setTexture(Urho3D::Texture2D* texture);
	void setProgress(float progress);
	void setData(QueueElement* _element);
	void reduce(short amount);
	static QueueHudElement* getFromElement(Urho3D::VariantMap& eventData);
private:
	Urho3D::Button* button;

	MySprite* icon;
	Urho3D::ProgressBar* bar;
	Urho3D::Text* text;
	Urho3D::UIElement* mock;

	QueueElement* element;
};

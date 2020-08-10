#pragma once

#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/ProgressBar.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIElement.h>


class QueueElement;
class MySprite;

class QueueHudElement {
public:
	explicit QueueHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style);
	~QueueHudElement();
	Urho3D::Button* getButton() const;
	void hide() const;
	void show() const;

	void setText(const Urho3D::String& msg) const;
	void hideText() const;
	void setTexture(Urho3D::Texture2D* texture) const;
	void setProgress(float progress) const;
	void setData(QueueElement* _element);
	void reduce(short amount) const;
	static QueueHudElement* getFromElement(Urho3D::VariantMap& eventData);
private:
	Urho3D::Button* button;

	MySprite* icon;
	Urho3D::ProgressBar* bar;
	Urho3D::Text* text;

	QueueElement* element;
};

#pragma once
#include <Urho3D/Core/Variant.h>


namespace Urho3D {
	class Text;
	class ProgressBar;
	class Texture2D;
	class String;
	class XMLFile;
	class UIElement;
	class Button;
}

class QueueElement;
class MySprite;

class QueueHudElement {
public:
	explicit QueueHudElement(Urho3D::UIElement* parent, Urho3D::XMLFile* style);
	~QueueHudElement() = default;
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

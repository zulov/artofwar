#pragma once
#include <Urho3D/UI/UIElement.h>

enum class MenuAction : char;

class HudData
{
public:
	explicit HudData(Urho3D::UIElement* _uiParent);
	~HudData();

	void set(short _id, MenuAction _type, Urho3D::String _text);
	static HudData* getFromElement(Urho3D::VariantMap& eventData);

	Urho3D::String& getText() { return text; }
	Urho3D::UIElement* getUIParent() const { return parent; }
	MenuAction getType() const { return type; }
	short getId() const { return id; }
private:
	Urho3D::UIElement* parent;
	Urho3D::String text;
	MenuAction type;
	short id;
};

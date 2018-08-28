#pragma once
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/UIEvents.h>

enum class MenuAction : char;

class HudData
{
public:
	explicit HudData(Urho3D::UIElement* _uiParent);
	~HudData();

	void set(short _id, MenuAction _type, Urho3D::String _text);

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

static HudData* getElement(Urho3D::VariantMap& eventData) {
	const auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	return static_cast<HudData *>(element->GetVar("HudElement").GetVoidPtr());
}

#pragma once
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/UIEvents.h>

enum class MenuAction : char;

class HudData
{
public:
	explicit HudData(Urho3D::UIElement* _uiParent);
	~HudData();

	void setId(short _id);
	void setId(short _id, MenuAction _type);
	MenuAction getType() const;
	short getId() const { return id; }
	Urho3D::UIElement* getUIParent() const { return parent; }
private:
	Urho3D::UIElement* parent;
	MenuAction type;
	short id;
};

static HudData* getElement(Urho3D::VariantMap& eventData) {
	const auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	return static_cast<HudData *>(element->GetVar("HudElement").GetVoidPtr());
}

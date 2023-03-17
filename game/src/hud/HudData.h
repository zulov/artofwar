#pragma once
#include <Urho3D/Core/Variant.h>

namespace Urho3D {
	class String;
	class UIElement;
}

enum class ActionType : char;

class HudData {
public:
	explicit HudData(Urho3D::UIElement* _uiParent);
	~HudData() = default;

	void set(short _id, ActionType _type, Urho3D::String _text);
	static HudData* getFromElement(Urho3D::VariantMap& eventData);

	Urho3D::String& getText() { return text; }
	Urho3D::UIElement* getUIParent() const { return parent; }
	ActionType getType() const { return type; }
	short getId() const { return id; }
private:
	Urho3D::UIElement* parent;
	Urho3D::String text;
	short id;
	ActionType type;
};

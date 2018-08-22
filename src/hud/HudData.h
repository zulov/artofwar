#pragma once
#include <Urho3D/UI/UIElement.h>

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

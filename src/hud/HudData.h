#pragma once
#include "hud/window/menu/LeftMenuAction.h"
#include <Urho3D/UI/UIElement.h>


class HudData
{
public:
	HudData(Urho3D::UIElement * _uiParent);
	~HudData();
	Urho3D::UIElement * getUIParent();

	void setId(short _id);
	void setId(short _id, LeftMenuAction _type);
	short getId();
	LeftMenuAction getType();
private:
	Urho3D::UIElement * parent;
	LeftMenuAction type;
	short id;

};


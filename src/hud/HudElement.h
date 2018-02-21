#pragma once
#include <Urho3D/UI/UIElement.h>
#include "ObjectEnums.h"

class HudElement
{
public:
	HudElement(Urho3D::UIElement * _uiElement);
	~HudElement();
	Urho3D::UIElement * getUIElement();

	void setId(short id, ObjectType _type);
	void setSubType(short _subType);
	short getId();
	short getSubType();
	ObjectType getType();
private:
	Urho3D::UIElement * uiElement;
	ObjectType type;
	short subType;
	short objectId;

};


#pragma once
#include <Urho3D/UI/UIElement.h>
#include "objects/unit/UnitType.h"
#include "objects/building/BuildingType.h"
#include "ObjectEnums.h"

class HudElement
{
public:
	HudElement(Urho3D::UIElement * _uiElement);
	~HudElement();
	Urho3D::UIElement * getUIElement();

	void setId(short id, ObjectType _type);
	short getId();
	ObjectType getType();
private:
	Urho3D::UIElement * uiElement;
	ObjectType type;
	short objectId;
};


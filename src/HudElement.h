#pragma once
#include <Urho3D/UI/UIElement.h>
#include "UnitType.h"
#include "BuildingType.h"

class HudElement
{
public:
	HudElement(Urho3D::UIElement * _uiElement);
	~HudElement();
	Urho3D::UIElement * getUIElement();

	UnitType getUnitType() const;
	void setUnitType(UnitType unitType);
	BuildingType getBuildingType() const;
	void setBuildingType(BuildingType buildingType);

	bool isUnitType1() const;
	bool isBuildType1() const;
private:
	Urho3D::UIElement * uiElement;
	UnitType unitType;
	BuildingType buildingType;
	bool isUnitType = false;
	bool isBuildType = false;
};


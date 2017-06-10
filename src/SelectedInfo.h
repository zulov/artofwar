#pragma once
#include "UnitType.h"
#include "ObjectEnums.h"
#include "BuildingType.h"
#include <Urho3D/Container/Str.h>
#define SELECTED_INFO_SIZE 10

class SelectedInfo
{
public:
	SelectedInfo();
	~SelectedInfo();
	Urho3D::String* getMessage();
	bool hasChanged();

	void setAllNumber(int allNumber);
	void setSelectedType(ObjectType selectedType);
	void setUnitsTypes(UnitType unitTypes[]);
	void setBuildingTypes(BuildingType buildingTypes[]);
private:
	bool changed = false;
	Urho3D::String* message;
	int allNumber;
	ObjectType selectedType;
	UnitType unitTypes[SELECTED_INFO_SIZE];
	BuildingType buildingTypes[SELECTED_INFO_SIZE];
	int number[SELECTED_INFO_SIZE];
};

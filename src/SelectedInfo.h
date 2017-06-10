#pragma once
#include "UnitType.h"
#include "ObjectEnums.h"
#include "BuildingType.h"
#include <Urho3D/Container/Str.h>
#define SELECTED_INFO_SIZE 10

class SelectedInfo
{
public:
	void initLines();
	void clearLines();
	SelectedInfo();
	~SelectedInfo();
	Urho3D::String* getMessage();
	Urho3D::String** getLines();
	bool hasChanged();

	void setAllNumber(int allNumber);
	void setSelectedType(ObjectType selectedType);
	void setNumberAt(int type, int value);
	void incNumberAt(int type);
	void setUnitsTypes(UnitType unitTypes[]);
	void setBuildingTypes(BuildingType buildingTypes[]);
	void reset();
private:
	bool changed = false;
	Urho3D::String* message;
	int allNumber;
	ObjectType selectedType;
	int number[SELECTED_INFO_SIZE];
	Urho3D::String** lines;

};

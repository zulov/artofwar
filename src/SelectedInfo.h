#pragma once
#include "UnitType.h"
#include "ObjectEnums.h"
#include "BuildingType.h"
#include <Urho3D/Container/Str.h>
#include "SelectedInfoType.h"

#define MAX_SELECTEDIN_TYPE 330

class SelectedInfo
{
public:
	void initLines();
	void clearLines();
	SelectedInfo();
	~SelectedInfo();
	Urho3D::String* getMessage();
	
	bool hasChanged();

	void setAllNumber(int allNumber);
	void setSelectedType(ObjectType selectedType);
	ObjectType getSelectedType();
	
	void reset();
	SelectedInfoType** getSelecteType();
	void select(Physical* entity);
private:
	bool changed = false;
	Urho3D::String* message;
	int allNumber;
	ObjectType selectedType;
	SelectedInfoType** selectedByType;
	Urho3D::String** lines;
	double **hps;
};

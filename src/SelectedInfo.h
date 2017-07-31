#pragma once
#include "UnitType.h"
#include "ObjectEnums.h"
#include "BuildingType.h"
#include <Urho3D/Container/Str.h>
#define SELECTED_INFO_SIZE 10
#define SELECTED_INFO_SIZE_2 330

class SelectedInfo
{
public:
	void initLines();
	void clearLines();
	SelectedInfo();
	~SelectedInfo();
	Urho3D::String* getMessage();
	Urho3D::String** getLines();
	double** getHps();
	bool hasChanged();

	void setAllNumber(int allNumber);
	void setSelectedType(ObjectType selectedType);
	ObjectType getSelectedType();
	void setNumberAt(int type, int value);
	void incNumberAt(int type);
	void reset();
private:
	bool changed = false;
	Urho3D::String* message;
	int allNumber;
	ObjectType selectedType;
	int number[SELECTED_INFO_SIZE];
	Urho3D::String** lines;
	double **hps;
};

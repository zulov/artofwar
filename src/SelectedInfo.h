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
	void clear();
	SelectedInfo();
	~SelectedInfo();
	bool hasChanged();
	void setAllNumber(int allNumber);
	void setSelectedType(ObjectType selectedType);
	ObjectType getSelectedType();
	int getAllNumber();
	void reset();
	SelectedInfoType** getSelecteType();
	void select(Physical* entity);
private:
	bool changed = false;
	int allNumber;
	ObjectType selectedType;
	SelectedInfoType** selectedByType;
};

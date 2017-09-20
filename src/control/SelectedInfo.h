#pragma once
#include "objects/unit/UnitType.h"
#include "ObjectEnums.h"
#include "objects/building//BuildingType.h"
#include <Urho3D/Container/Str.h>
#include "SelectedInfoType.h"

#define MAX_SELECTEDIN_TYPE 330

class SelectedInfo
{
public:
	SelectedInfo();
	~SelectedInfo();
	bool hasChanged();
	void setAllNumber(int allNumber);
	void setSelectedType(ObjectType selectedType);
	ObjectType getSelectedType();
	int getAllNumber();
	void reset();
	std::vector<SelectedInfoType*>* getSelecteType();
	void select(Physical* entity);
	int getSelectedSubTypeNumber();
	void hasBeedUpdatedDrawn();
private:
	bool changed = false;
	int allNumber;
	int allSubTypeNumber;
	ObjectType selectedType;
	std::vector<SelectedInfoType*>* selectedByType;
};
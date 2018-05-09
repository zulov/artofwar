#pragma once
#include "ObjectEnums.h"
#include "SelectedInfoType.h"
#include <Urho3D/Container/Str.h>

#define MAX_SELECTEDIN_TYPE 330
#define MAX_SIZE_TYPES 8

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
	std::vector<SelectedInfoType*>& getSelectedTypes();
	void select(Physical* entity);
	char getSelectedSubTypeNumber();
	void hasBeedUpdatedDrawn();
	String& getMessage();
	void setMessage(String& s);
private:
	bool changed = true;
	int allNumber;
	char allSubTypeNumber;
	ObjectType selectedType;
	std::vector<SelectedInfoType*> selectedByType;
	String message = "";
};

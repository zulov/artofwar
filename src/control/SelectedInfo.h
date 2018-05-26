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
	void setAllNumber(int allNumber);
	void setSelectedType(ObjectType selectedType);
	void reset();
	void select(Physical* entity);
	void hasBeedUpdatedDrawn();
	void setMessage(Urho3D::String& s);

	std::vector<SelectedInfoType*>& getSelectedTypes() { return selectedByType; }
	ObjectType getSelectedType() const {return selectedType;}
	Urho3D::String& getMessage() { return message; }
	char getSelectedSubTypeNumber() const { return allSubTypeNumber; }
	int getAllNumber() const { return allNumber; }
	bool hasChanged() const { return changed; }
private:
	bool changed = true;
	int allNumber;
	char allSubTypeNumber;
	ObjectType selectedType;
	std::vector<SelectedInfoType*> selectedByType;
	Urho3D::String message = "";
};

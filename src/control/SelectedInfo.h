#pragma once

#include <optional>
#include <vector>

#define MAX_SELECTED_TYPE 330
#define MAX_SIZE_TYPES 8

class Physical;
enum class ObjectType : char;
class SelectedInfoType;

class SelectedInfo
{
public:
	SelectedInfo();
	~SelectedInfo();
	void setAllNumber(int allNumber);
	void setSelectedType(ObjectType selectedType);
	void reset();
	void select(Physical* entity);
	void hasBeenUpdatedDrawn();
	std::optional<SelectedInfoType*> getOneSelectedTypeInfo() const;

	std::vector<SelectedInfoType*>& getSelectedTypes() { return selectedByType; }
	ObjectType getSelectedType() const { return selectedType; }
	char getSelectedSubTypeNumber() const { return allSubTypeNumber; }
	int getAllNumber() const { return allNumber; }
	bool hasChanged() const { return changed; }
	bool isSthSelected();
	void refresh(std::vector<Physical*>* selected);
private:
	bool changed = true;
	int allNumber;
	char allSubTypeNumber;
	ObjectType selectedType;
	std::vector<SelectedInfoType*> selectedByType;
};

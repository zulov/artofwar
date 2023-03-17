#pragma once

#include <optional>
#include <vector>

class Physical;
enum class ObjectType : char;
class SelectedInfoType;

class SelectedInfo {
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
	bool isSthSelected() const;
	void refresh(const std::vector<Physical*>& selected);
private:
	std::vector<SelectedInfoType*> selectedByType;
	int allNumber;
	bool changed = true;
	char allSubTypeNumber;
	ObjectType selectedType;
};

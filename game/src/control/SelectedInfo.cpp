#include "SelectedInfo.h"

#include "Game.h"
#include "SelectedInfoType.h"
#include "database/DatabaseCache.h"
#include "objects/Physical.h"
#include "utils/DeleteUtils.h"


SelectedInfo::SelectedInfo() {
	allNumber = 0;
	allSubTypeNumber = 0;
	const auto db = Game::getDatabase();

	const auto size = Urho3D::Max(db->getUnits().size(), db->getBuildings().size());
	selectedByType.reserve(size);

	for (int i = 0; i < size; ++i) {
		selectedByType.push_back(new SelectedInfoType(i));
	}
}

SelectedInfo::~SelectedInfo() {
	clear_vector(selectedByType);
}

void SelectedInfo::setAllNumber(int allNumber) {
	this->allNumber = allNumber;
	changed = true;
}

void SelectedInfo::setSelectedType(ObjectType selectedType) {
	this->selectedType = selectedType;
	changed = true;
}

void SelectedInfo::reset() {
	changed = true;
	allNumber = 0;
	allSubTypeNumber = 0;
	for (auto element : selectedByType) {
		element->clear();
	}
}

void SelectedInfo::select(Physical* entity) {
	auto id = entity->getId();
	if (id >= 0) {
		selectedByType.at(id)->add(entity);
		if (selectedByType.at(id)->getData().size() == 1) {
			++allSubTypeNumber;
		}
	}
	changed = true;
}

void SelectedInfo::hasBeenUpdatedDrawn() {
	changed = false;
}

std::optional<SelectedInfoType*> SelectedInfo::getOneSelectedTypeInfo() const {
	for (auto info : selectedByType) {
		if (!info->getData().empty()) {
			return info;
		}
	}
	return {};
}

bool SelectedInfo::isSthSelected() const {
	return allNumber > 0 && selectedType != ObjectType::NONE;
}

void SelectedInfo::refresh(const std::vector<Physical*>& selected) {
	reset();
	for (auto physical : selected) {
		select(physical);
	}
}

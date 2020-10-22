#include "LeftMenuInfoPanel.h"
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "hud/HudData.h"
#include "hud/UiUtils.h"
#include "objects/ActionType.h"
#include "objects/NamesCache.h"
#include "objects/Physical.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "utils/NamesUtils.h"


LeftMenuInfoPanel::LeftMenuInfoPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "LeftMenuInfoPanel", {}) {
}

void LeftMenuInfoPanel::createBody() {
	text = addChildText(window, "MyText", style);
}

void LeftMenuInfoPanel::updateSelected(SelectedInfo* selectedInfo) {
	if (!hoverIsOn) {
		selectedIsOn = true;
		if (selectedInfo->getAllNumber() == 1) {
			auto optInfoTypes = selectedInfo->getOneSelectedTypeInfo();
			if (optInfoTypes.has_value()) {
				text->SetText(optInfoTypes.value()->getData().at(0)->toMultiLineString());
				setVisible(true);
			}
		} else if (selectedInfo->isSthSelected()) {
			Urho3D::String msg = "";
			for (auto& selectedType : selectedInfo->getSelectedTypes()) {
				if (!selectedType->getData().empty()) {
					msg.Append(getName(selectedType->getData().at(0)->getType(),
					                   selectedType->getData().at(0)->getId()))
					   .Append(": ").Append(Urho3D::String(selectedType->getData().size())).Append("\n");
				}
			}
			text->SetText(msg);
			setVisible(true);
		} else {
			setVisible(false);
		}
	}
}

Urho3D::String LeftMenuInfoPanel::createMessage(HudData* hudData) {
	const auto id = hudData->getId();
	switch (hudData->getType()) {
	case ActionType::UNIT_CREATE:
	{
		auto dbUnit = Game::getDatabase()->getUnit(id);

		return stringFrom(dbUnit->name, dbUnit->costs);
	}
	case ActionType::UNIT_LEVEL:
	{
		auto opt = Game::getPlayersMan()->getActivePlayer()->getNextLevelForUnit(id); 
		if (opt.has_value()) {
			auto dbLevel = opt.value();
			return stringFrom(dbLevel->name, dbLevel->costs);
		}
	}
	case ActionType::UNIT_UPGRADE:
		return "TODO";
	case ActionType::BUILDING_CREATE:
	{
		auto dbBuilding = Game::getDatabase()->getBuilding(id);

		return stringFrom(dbBuilding->name, dbBuilding->costs);
	}
	case ActionType::BUILDING_LEVEL:
	{
		auto opt = Game::getPlayersMan()->getActivePlayer()->getNextLevelForBuilding(id); 

		if (opt.has_value()) {
			auto dbLevel = opt.value();
			return stringFrom(dbLevel->name, dbLevel->costs);
		}
	}
	case ActionType::ORDER:
		return Game::getLocalization()->Get(Game::getDatabase()->getOrder(id)->name);
	case ActionType::FORMATION:
	case ActionType::RESOURCE:
		return hudData->getText();
	default: ;
	}
}

void LeftMenuInfoPanel::setHoverInfo(HudData* hudData) {
	Urho3D::String message = createMessage(hudData);

	if (message.Length() > 0) {
		text->SetText(message);
		setVisible(true);
		hoverIsOn = true;
	} else {
		removeHoverInfo();
	}

}

void LeftMenuInfoPanel::removeHoverInfo() {
	hoverIsOn = false;
	if (!selectedIsOn) {
		setVisible(false);
	}
}

void LeftMenuInfoPanel::clearSelected() {
	selectedIsOn = false;
	if (!hoverIsOn) {
		setVisible(false);
	}
}

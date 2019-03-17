#include "LeftMenuInfoPanel.h"
#include "Game.h"
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "database/DatabaseCache.h"
#include "database/db_strcut.h"
#include "hud/HudData.h"
#include "hud/UiUtils.h"
#include "objects/ActionType.h"
#include "objects/Physical.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "NamesUtils.h"


LeftMenuInfoPanel::LeftMenuInfoPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "LeftMenuInfoPanel", {}) {
}

LeftMenuInfoPanel::~LeftMenuInfoPanel() = default;


void LeftMenuInfoPanel::createBody() {
	Urho3D::String a = "";
	text = addChildText(window, "MyText", a, style);
	text2 = addChildText(window, "MyText", a, style);
}

void LeftMenuInfoPanel::updateSelected(SelectedInfo* selectedInfo) {
	if (!hoverIsOn) {
		selectedIsOn = true;
		setVisible(true);
		if (selectedInfo->getAllNumber() == 1) {
			text->SetVisible(true);
			auto& infoTypes = selectedInfo->getSelectedTypes();
			for (auto& infoType : infoTypes) {
				auto& data = infoType->getData();
				if (!data.empty()) {
					text->SetText(data.at(0)->toMultiLineString());
					break;
				}
			}
		} else {
			text->SetVisible(false);
		}

		text2->SetText(selectedInfo->getMessage());
		text2->SetVisible(true);
	}
}

Urho3D::String LeftMenuInfoPanel::createMessage(HudData* hudData) {
	const auto id = hudData->getId();
	switch (hudData->getType()) {
	case ActionType::UNIT_CREATE:
		{
		return stringFrom(Game::getDatabaseCache()->getUnit(id)->name,
		                  Game::getDatabaseCache()->getCostForUnit(id));
		}
	case ActionType::UNIT_LEVEL:
		{
		const int level = Game::getPlayersMan()->getActivePlayer()->getLevelForUnit(id) + 1;
		const auto dbLevel = Game::getDatabaseCache()->getUnitLevel(id, level).value();
		auto opt = Game::getDatabaseCache()->getCostForUnitLevel(id, level);

		return stringFrom(dbLevel->name, opt.value());
		}
	case ActionType::UNIT_UPGRADE:
		{
		return "TODO";
		}
	case ActionType::BUILDING_CREATE:
		{
		return stringFrom(Game::getDatabaseCache()->getBuilding(id)->name,
		                  Game::getDatabaseCache()->getCostForBuilding(id));
		}
	case ActionType::BUILDING_LEVEL:
		{
		auto level = Game::getPlayersMan()->getActivePlayer()->getLevelForBuilding(id) + 1;
		auto dbLevel = Game::getDatabaseCache()->getBuildingLevel(id, level).value();
		auto optCost = Game::getDatabaseCache()->getCostForUnitLevel(id, level);

		return stringFrom(dbLevel->name, optCost.value());
		}
	case ActionType::ORDER:
		{
		return Game::getLocalization()->Get(Game::getDatabaseCache()->getOrder(id)->name);
		}
		break;
	case ActionType::FORMATION:
	case ActionType::RESOURCE:
		return hudData->getText();
	default: ;
	}
}

void LeftMenuInfoPanel::setHoverInfo(HudData* hudData) {
	Urho3D::String message = createMessage(hudData);

	if (message.Length() > 0) {
		text->SetVisible(true);
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

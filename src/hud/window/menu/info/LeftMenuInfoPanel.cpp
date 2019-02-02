#include "LeftMenuInfoPanel.h"
#include "Game.h"
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "database/DatabaseCache.h"
#include "database/db_strcut.h"
#include "hud/HudData.h"
#include "hud/UiUtils.h"
#include "objects/MenuAction.h"
#include "objects/Physical.h"
#include "player/Player.h"
#include "player/PlayersManager.h"


LeftMenuInfoPanel::LeftMenuInfoPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "LeftMenuInfoPanel", {}) {
}

LeftMenuInfoPanel::~LeftMenuInfoPanel() = default;


void LeftMenuInfoPanel::createBody() {
	Urho3D::String a = "";
	text = addChildText(window, "MyText", a, style);
	text2 = addChildText(window, "MyText", a, style);
}

void LeftMenuInfoPanel::updateSelected(SelectedInfo* selectedInfo) {
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

void LeftMenuInfoPanel::setInfo(HudData* hudData) {
	Urho3D::String message = "";
	const auto id = hudData->getId();
	switch (hudData->getType()) {

	case MenuAction::UNIT_CREATE:
		{
		auto dbUnit = Game::getDatabaseCache()->getUnit(id);
		auto costs = Game::getDatabaseCache()->getCostForUnit(id);
		message = stringFrom(dbUnit->name, costs);
		}
		break;
	case MenuAction::UNIT_LEVEL:
		{
		const int level = Game::getPlayersMan()->getActivePlayer()->getLevelForUnit(id) + 1;
		auto dbLevel = Game::getDatabaseCache()->getUnitLevel(id, level).value();
		auto opt = Game::getDatabaseCache()->getCostForUnitLevel(id, level);

		message = stringFrom(dbLevel->name, opt.value());
		}
		break;
	case MenuAction::UNIT_UPGRADE:
		{
		message = "TODO";
		}
		break;
	case MenuAction::BUILDING:
		{
		auto dbBuilding = Game::getDatabaseCache()->getBuilding(id);
		auto costs = Game::getDatabaseCache()->getCostForBuilding(id);
		message = stringFrom(dbBuilding->name, costs);
		}
		break;
	case MenuAction::BUILDING_LEVEL:
		{
		auto level = Game::getPlayersMan()->getActivePlayer()->getLevelForBuilding(id) + 1;
		auto dbLevel = Game::getDatabaseCache()->getBuildingLevel(id, level).value();
		auto opt = Game::getDatabaseCache()->getCostForUnitLevel(id, level);
		auto costs = opt.value();
		message = stringFrom(dbLevel->name, costs);
		}
		break;
	case MenuAction::ORDER:
		{
		message = Game::getLocalization()->Get(Game::getDatabaseCache()->getOrder(id)->name);
		}
		break;
	case MenuAction::FORMATION:
	case MenuAction::RESOURCE:
		message = hudData->getText();
		break;
	default: ;
	}

	if (message.Length() > 0) {
		text->SetVisible(true);
		text->SetText(message);
		setVisible(true);
	} else {
		setVisible(false);
	}

}

Urho3D::String LeftMenuInfoPanel::stringFrom(const Urho3D::String& name, std::vector<db_cost*>* costs) {
	auto msg = name + "\n";
	for (db_cost* cost : *costs) {
		msg += cost->resourceName + " - " + Urho3D::String(cost->value) + "\n";
	}
	return msg;
}

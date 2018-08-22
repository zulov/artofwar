#include "LeftMenuInfoPanel.h"
#include "Game.h"
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "database/DatabaseCache.h"
#include "database/db_strcut.h"
#include "hud/HudData.h"
#include "objects/Physical.h"
#include "player/Player.h"
#include "player/PlayersManager.h"


LeftMenuInfoPanel::LeftMenuInfoPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "LeftMenuInfoPanel";
}

LeftMenuInfoPanel::~LeftMenuInfoPanel() = default;


void LeftMenuInfoPanel::createBody() {
	text = window->CreateChild<Urho3D::Text>();
	text->SetStyle("MyText", style);
	text->SetText("");

	text2 = window->CreateChild<Urho3D::Text>();
	text2->SetStyle("MyText", style);
	text2->SetText("");
}

void LeftMenuInfoPanel::updateSelected(SelectedInfo* selectedInfo) {
	setVisible(true);
	if (selectedInfo->getAllNumber() == 1) {
		text->SetVisible(true);
		auto& infoTypes = selectedInfo->getSelectedTypes();
		for (auto& infoType : infoTypes) {
			auto& data = infoType->getData();
			if (!data.empty()) {
				Physical* physical = data.at(0);

				text->SetText(physical->toMultiLineString());
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
	const short id = hudData->getId();
	LeftMenuAction type = hudData->getType();

	switch (type) {

	case LeftMenuAction::UNIT:
		{
		db_unit* dbUnit = Game::getDatabaseCache()->getUnit(id);
		auto costs = Game::getDatabaseCache()->getCostForUnit(id);
		message = stringFrom(dbUnit->name, costs);
		}
		break;
	case LeftMenuAction::UNIT_LEVEL:
		{
		int level = Game::getPlayersManager()->getActivePlayer()->getLevelForUnit(id) + 1;
		db_unit_level* dbLevel = Game::getDatabaseCache()->getUnitLevel(id, level).value();
		auto opt = Game::getDatabaseCache()->getCostForUnitLevel(id, level);
		auto costs = opt.value();
		message = stringFrom(dbLevel->name, costs);
		}
		break;
	case LeftMenuAction::UNIT_UPGRADE:
		{
		message = "TODO";
		}
		break;
	case LeftMenuAction::BUILDING:
		{
		db_building* dbBuilding = Game::getDatabaseCache()->getBuilding(id);
		auto costs = Game::getDatabaseCache()->getCostForBuilding(id);
		message = stringFrom(dbBuilding->name, costs);
		}
		break;
	case LeftMenuAction::BUILDING_LEVEL:
		{
		int level = Game::getPlayersManager()->getActivePlayer()->getLevelForBuilding(id) + 1;
		auto dbLevel = Game::getDatabaseCache()->getBuildingLevel(id, level).value();
		auto opt = Game::getDatabaseCache()->getCostForUnitLevel(id, level);
		auto costs = opt.value();
		message = stringFrom(dbLevel->name, costs);
		}
		break;
	case LeftMenuAction::BUILDING_UPGRADE:
		{
		message = "TODO";
		}
		break;
	case LeftMenuAction::ORDER:
		{
		db_order* dbOrder = Game::getDatabaseCache()->getOrder(id);
		message = Game::getLocalization()->Get(dbOrder->name);
		}
		break;
	case LeftMenuAction::FORMATION:
		message = "TODO";
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
	Urho3D::String msg = name + "\n";
	for (db_cost* cost : *costs) {
		msg += cost->resourceName + " - " + Urho3D::String(cost->value) + "\n";
	}
	return msg;
}

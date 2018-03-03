#include "LeftMenuInfoPanel.h"
#include "control/SelectedInfo.h"
#include "hud/HudData.h"
#include "database/db_strcut.h"
#include "Game.h"
#include "database/DatabaseCache.h"


LeftMenuInfoPanel::LeftMenuInfoPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "LeftMenuInfoPanel";
}

LeftMenuInfoPanel::~LeftMenuInfoPanel() {
}


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
		std::vector<SelectedInfoType*>& infoTypes = selectedInfo->getSelectedTypes();
		for (auto& infoType : infoTypes) {
			std::vector<Physical*>& data = infoType->getData();
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
	String s = "";
	const short id = hudData->getId();
	LeftMenuAction type = hudData->getType();

	switch (type) {

	case LeftMenuAction::UNIT:
		{
		db_unit* dbUnit = Game::get()->getDatabaseCache()->getUnit(id);
		std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForUnit(id);
		s = stringFrom(dbUnit, costs);
		}
		break;
	case LeftMenuAction::UNIT_LEVEL:
		s = "TODO";
		break;
	case LeftMenuAction::UNIT_UPGRADE:
		s = "TODO";
		break;
	case LeftMenuAction::BUILDING:
		{
		db_building* dbBuilding = Game::get()->getDatabaseCache()->getBuilding(id);
		std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForBuilding(id);
		s = stringFrom(dbBuilding, costs);
		}
		break;
	case LeftMenuAction::BUILDING_LEVEL:
	case LeftMenuAction::BUILDING_UPGRADE:
	case LeftMenuAction::ORDER:
	case LeftMenuAction::FORMATION:
		s = "TODO";
		break;
	default: ;
	}


	switch (type) {

	case ObjectType::UNIT:
		{
		}
		break;
	case ObjectType::BUILDING:
		{
		}
		break;
	default: ;
	}
	if (s.Length() > 0) {
		text->SetVisible(true);
		text->SetText(s);
		setVisible(true);
	} else {
		setVisible(false);
	}

}

Urho3D::String LeftMenuInfoPanel::stringFrom(db_unit* dbUnit, std::vector<db_cost*>* costs) {
	String msg = dbUnit->name + "\n";
	for (db_cost* cost : (*costs)) {
		msg += cost->resourceName + " - " + String(cost->value) + "\n";
	}
	return msg;
}

Urho3D::String LeftMenuInfoPanel::stringFrom(db_building* dbBuilding, std::vector<db_cost*>* costs) {
	String msg = dbBuilding->name + "\n";
	for (db_cost* cost : (*costs)) {
		msg += cost->resourceName + " - " + String(cost->value) + "\n";
	}
	return msg;
}

#include "MenuPanel.h"
#include "Game.h"
#include "database/DatabaseCache.h"


MenuPanel::MenuPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "MenuWindow";
}


MenuPanel::~MenuPanel() {
}

void MenuPanel::removeInfo() {
	text->SetVisible(false);
	text2->SetVisible(false);
}

Urho3D::String MenuPanel::stringFrom(db_unit* dbUnit, std::vector<db_cost*>* costs) {
	String msg = dbUnit->name + "\n";
	for (db_cost* cost : (*costs)) {
		msg += cost->resourceName + " - " + String(cost->value) + "\n";
	}
	return msg;
}

Urho3D::String MenuPanel::stringFrom(db_building* dbBuilding, std::vector<db_cost*>* costs) {
	String msg = dbBuilding->name + "\n";
	for (db_cost* cost : (*costs)) {
		msg += cost->resourceName + " - " + String(cost->value) + "\n";
	}
	return msg;
}

void MenuPanel::setInfo(HudElement* hudElement) {
	String s = "";
	ObjectType type = hudElement->getType();
	short id = hudElement->getId();
	switch (type) {

	case UNIT:
		{
		db_unit* dbUnit = Game::get()->getDatabaseCache()->getUnit(id);
		std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForUnit(id);
		s = stringFrom(dbUnit, costs);
		}
		break;
	case BUILDING:
		{
		db_building* dbBuilding = Game::get()->getDatabaseCache()->getBuilding(id);
		std::vector<db_cost*>* costs = Game::get()->getDatabaseCache()->getCostForBuilding(id);
		s = stringFrom(dbBuilding, costs);
		}
		break;
	default: ;
	}
	text->SetVisible(true);
	text->SetText(s);

}

void MenuPanel::updateSelected(SelectedInfo* selectedInfo) {
	if(selectedInfo->getAllNumber()==1) {
		text->SetVisible(true);
		std::vector<SelectedInfoType*>* infoTypes = selectedInfo->getSelecteType();
		for (int i = 0; i < infoTypes->size(); ++i) {
			std::vector<Physical*>* data = infoTypes->at(i)->getData();
			if (!data->empty()) {
				Physical* physical = data->at(0);
				String *msg = physical->toMultiLineString();
				text->SetText(*msg);
				break;
			}
		}
	}else {
		text->SetVisible(false);
	}
	String *msg = selectedInfo->getMessage();
	text2->SetText(*msg);
	text2->SetVisible(true);
}

void MenuPanel::createBody() {
	text = window->CreateChild<Urho3D::Text>();
	text->SetStyle("MyText", style);
	text->SetText("");

	text2 = window->CreateChild<Urho3D::Text>();
	text2->SetStyle("MyText", style);
	text2->SetText("");
}

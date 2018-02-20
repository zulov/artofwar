#include "MenuPanel.h"
#include "Game.h"
#include "GameState.h"
#include "database/DatabaseCache.h"
#include <Urho3D/Resource/ResourceCache.h>
#include "hud/MySprite.h"
#include "hud/UiUtils.h"
#include <Urho3D/UI/CheckBox.h>


MenuPanel::MenuPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "LeftMenuWindow";
	visibleAt.insert(GameState::RUNNING);
	visibleAt.insert(GameState::PAUSE);
}


MenuPanel::~MenuPanel() = default;

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
	const short id = hudElement->getId();
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
	if (selectedInfo->getAllNumber() == 1) {
		text->SetVisible(true);
		std::vector<SelectedInfoType*>& infoTypes = selectedInfo->getSelecteType();
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

void MenuPanel::createBody() {
	mock = window->CreateChild<UIElement>();
	mock->SetStyle("LeftMenuMock", style);

	for (int i = 0; i < LEFT_MENU_ROWS_NUMBER; ++i) {
		rows[i] = mock->CreateChild<UIElement>();
		rows[i]->SetStyle("LeftMenuListRow", style);
	}
	for (int i = 0; i < LEFT_MENU_CHECKS_NUMBER; ++i) {
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D
		>("textures/hud/icon/lm/lm" + String(i) + ".png");

		MySprite* sprite = createSprite(texture, style, "LeftMenuSmallSprite");
		checks[i] = rows[LEFT_MENU_ROWS_NUMBER-1]->CreateChild<CheckBox>();
		checks[i]->SetStyle("LeftMenuCheckBox", style);

		checks[i]->AddChild(sprite);
	}
	Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D
	>("textures/hud/icon/lm/lm3.png");
	MySprite* sprite = createSprite(texture, style, "LeftMenuSmallSprite");
	nextButton = rows[LEFT_MENU_ROWS_NUMBER-1]->CreateChild<Button>();
	nextButton->SetStyle("LeftMenuIcon", style);

	nextButton->AddChild(sprite);

	int k=0;
	for (int i = 0; i < LEFT_MENU_ROWS_NUMBER-1; ++i) {
		for (int j = 0; j < LEFT_MENU_BUTTON_PER_ROW; ++j) {

			buttons[k] = rows[i]->CreateChild<Button>();
			buttons[k]->SetStyle("LeftMenuBigIcon", style);
			k++;
		}
	}

	text = window->CreateChild<Urho3D::Text>();
	text->SetStyle("MyText", style);
	text->SetText("");

	text2 = window->CreateChild<Urho3D::Text>();
	text2->SetStyle("MyText", style);
	text2->SetText("");
}

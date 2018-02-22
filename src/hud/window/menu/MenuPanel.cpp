#include "MenuPanel.h"
#include "Game.h"
#include "GameState.h"
#include "database/DatabaseCache.h"
#include <Urho3D/Resource/ResourceCache.h>
#include "hud/MySprite.h"
#include "hud/UiUtils.h"
#include "info/LeftMenuInfoPanel.h"
#include <Urho3D/UI/CheckBox.h>
#include "hud/window/in_game_menu/middle/AbstractMiddlePanel.h"
#include "player/PlayersManager.h"
#include <unordered_set>
#include <Urho3D/UI/UIEvents.h>


MenuPanel::MenuPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "LeftMenuWindow";
	visibleAt.insert(GameState::RUNNING);
	visibleAt.insert(GameState::PAUSE);
}


MenuPanel::~MenuPanel() {
	delete infoPanel;
	clear_vector(hudElements);
}

void MenuPanel::removeInfo() {
	infoPanel->setVisible(false);
}

void MenuPanel::refresh(LeftMenuMode _mode, SelectedInfo* selectedInfo) {
	lastSelectedInfo = selectedInfo;
	if (mode != _mode) {
		mode = _mode;
		subMode = BASIC;
	}
	updateButtons(lastSelectedInfo);
}

void MenuPanel::setInfo(HudElement* hudElement) {
	infoPanel->setInfo(hudElement);
}

void MenuPanel::updateSelected(SelectedInfo* selectedInfo) {
	infoPanel->updateSelected(selectedInfo);
}

void MenuPanel::setVisible(bool enable) {
	AbstractWindowPanel::setVisible(enable);
	if (!enable) {
		infoPanel->setVisible(false);
	}
}

std::vector<HudElement*>& MenuPanel::getButtons() {
	return hudElements;
}

void MenuPanel::createBody() {
	infoPanel = new LeftMenuInfoPanel(style);
	infoPanel->createWindow();
	infoPanel->setVisible(false);

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
		checks[i] = rows[LEFT_MENU_ROWS_NUMBER - 1]->CreateChild<CheckBox>();
		checks[i]->SetStyle("LeftMenuCheckBox", style);
		checks[i]->SetVar("Num", i);
		checks[i]->AddChild(sprite);
		SubscribeToEvent(checks[i], E_CLICK, URHO3D_HANDLER(MenuPanel, ChengeModeButton));
	}
	Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D
	>("textures/hud/icon/lm/lm3.png");
	MySprite* sprite = createSprite(texture, style, "LeftMenuSmallSprite");
	nextButton = rows[LEFT_MENU_ROWS_NUMBER - 1]->CreateChild<Button>();
	nextButton->SetStyle("LeftMenuIcon", style);
	nextButton->AddChild(sprite);

	int k = 0;
	for (int i = 0; i < LEFT_MENU_ROWS_NUMBER - 1; ++i) {
		for (int j = 0; j < LEFT_MENU_BUTTON_PER_ROW; ++j) {
			sprites[k] = createEmptySprite(style, "LeftMenuSprite");
			buttons[k] = simpleButton(rows[i], sprites[k], style, "LeftMenuBigIcon");
			hudElements.push_back(new HudElement(buttons[k]));
			hudElements[k]->setId(-1, ENTITY);
			hudElements[k]->setSubType(-1);
			buttons[k]->SetVar("HudElement", hudElements[k]);
			k++;
		}
	}

}

void MenuPanel::setChecks(int val) {
	for (auto check : checks) {
		check->SetChecked(false);
	}
	checks[val]->SetChecked(true);
}

void MenuPanel::ChengeModeButton(StringHash eventType, VariantMap& eventData) {
	CheckBox* element = (CheckBox*)eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr();
	int val = element->GetVar("Num").GetInt();
	LeftMenuSubMode newSubMode = static_cast<LeftMenuSubMode>(val);
	if (newSubMode != subMode) {
		subMode = newSubMode;
		updateButtons(lastSelectedInfo);
	}
}

void MenuPanel::basicBuilding() {
	int size = Game::get()->getDatabaseCache()->getBuildingSize();
	int nation = Game::get()->getPlayersManager()->getActivePlayer()->getNation();
	int k = 0;
	for (int i = 0; i < size; ++i) {
		db_building* building = Game::get()->getDatabaseCache()->getBuilding(i);
		if (building) {
			if (building->nation == nation) {
				Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D
				>("textures/hud/icon/building/" + building->icon);
				setTextureToSprite(sprites[k], texture);

				hudElements[k]->setId(building->id, BUILDING);
				hudElements[k]->setSubType(subMode);
				k++;
			}
		}
	}
	resetButtons(k);
}

void MenuPanel::basicUnit(SelectedInfo* selectedInfo) {
	vector<SelectedInfoType*> infoTypes = selectedInfo->getSelecteType();

	unordered_set<int> common = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int nation = Game::get()->getPlayersManager()->getActivePlayer()->getNation();
	for (int i = 0; i < infoTypes.size(); ++i) {
		std::vector<Physical*>& data = infoTypes.at(i)->getData();
		if (!data.empty()) {
			std::vector<db_unit*>* units = Game::get()->getDatabaseCache()->getUnitsForBuilding(i);
			unordered_set<int> common2;
			for (auto& unit : *units) {
				//todo to zrobic raz i pobierac
				if (unit->nation == nation) {
					common2.insert(unit->id);
				}
			}
			unordered_set<int> temp(common);
			for (const auto& id : temp) {
				if (common2.find(id) == common2.end()) {
					common.erase(id);
				}
			}
		}
	}

	int size = Game::get()->getDatabaseCache()->getUnitSize();

	int k = 0;
	for (int i = 0; i < size; ++i) {
		if (common.find(i) != common.end()) {
			db_unit* unit = Game::get()->getDatabaseCache()->getUnit(i);
			if (unit) {
				if (unit->nation == nation) {
					Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D
					>("textures/hud/icon/unit/" + unit->icon);
					setTextureToSprite(sprites[k], texture);

					hudElements[k]->setId(unit->id, UNIT);
					hudElements[k]->setSubType(subMode);
					k++;
				}
			}
		}
	}
	resetButtons(k);
}

void MenuPanel::basicOrder(SelectedInfo* selectedInfo) {
	std::vector<SelectedInfoType*>& infoTypes = selectedInfo->getSelecteType();

	std::unordered_set<int> common = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	for (int i = 0; i < infoTypes.size(); ++i) {
		std::vector<Physical*>& data = infoTypes.at(i)->getData();
		if (!data.empty()) {
			std::vector<db_order*>* orders = Game::get()->getDatabaseCache()->getOrdersForUnit(i);
			unordered_set<int> common2;
			for (auto& order : *orders) {
				//todo to zrobic raz i pobierac
				common2.insert(order->id);
			}
			std::unordered_set<int> temp(common);
			for (const auto& id : temp) {
				if (common2.find(id) == common2.end()) {
					common.erase(id);
				}
			}
		}
	}

	int size = Game::get()->getDatabaseCache()->getOrdersSize();

	int k = 0;
	for (int i = 0; i < size; ++i) {
		if (common.find(i) != common.end()) {
			db_order* order = Game::get()->getDatabaseCache()->getOrder(i);
			if (order) {
				Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D
				>("textures/hud/icon/orders/" + order->icon);
				setTextureToSprite(sprites[k], texture);
				buttons[k]->SetVar("Num", order->id);

				hudElements[k]->setId(order->id, ENTITY);
				hudElements[k]->setSubType(subMode);
				k++;
			}
		}
	}
	resetButtons(k);
}

void MenuPanel::resetButtons(int from) {
	for (int i = from; i < LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1); ++i) {
		setTextureToSprite(sprites[i], nullptr);
		buttons[i]->SetVar("Num", -1);
	}
}

void MenuPanel::updateButtons(SelectedInfo* selectedInfo) {
	setChecks(subMode);
	switch (mode) {

	case LeftMenuMode::BUILDING:
		basicBuilding();
		break;
	case LeftMenuMode::UNIT:
		basicUnit(selectedInfo);
		break;
	case LeftMenuMode::ORDER:
		basicOrder(selectedInfo);
		break;
	default: ;
	}
}

#include "MenuPanel.h"
#include "GameState.h"
#include "database/DatabaseCache.h"
#include "hud/MySprite.h"
#include "hud/UiUtils.h"
#include "hud/window/in_game_menu/middle/AbstractMiddlePanel.h"
#include "info/LeftMenuInfoPanel.h"
#include "objects/building/BuildingUtils.h"
#include "player/PlayersManager.h"
#include "control/SelectedInfoType.h"
#include "utils.h"
#include "player/Player.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/UIEvents.h>
#include <unordered_set>
#include "LeftMenuAction.h"
#include "hud/HudData.h"
#include "control/SelectedInfo.h"

MenuPanel::MenuPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "LeftMenuWindow";
	visibleAt[static_cast<char>(GameState::RUNNING)] = true;
	visibleAt[static_cast<char>(GameState::PAUSE)] = true;
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

void MenuPanel::setInfo(HudData* hudData) {
	infoPanel->setInfo(hudData);
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

std::vector<HudData*>& MenuPanel::getButtons() {
	return hudElements;
}

void MenuPanel::createBody() {
	infoPanel = new LeftMenuInfoPanel(style);
	infoPanel->createWindow();
	infoPanel->setVisible(false);

	mock = window->CreateChild<Urho3D::UIElement>();
	mock->SetStyle("LeftMenuMock", style);

	for (auto& row : rows) {
		row = mock->CreateChild<Urho3D::UIElement>();
		row->SetStyle("LeftMenuListRow", style);
	}
	for (int i = 0; i < LEFT_MENU_CHECKS_NUMBER; ++i) {
		auto texture = Game::getCache()->GetResource<Urho3D::Texture2D
		>("textures/hud/icon/lm/lm" + Urho3D::String(i) + ".png");

		MySprite* sprite = createSprite(texture, style, "LeftMenuSmallSprite");
		checks[i] = rows[LEFT_MENU_ROWS_NUMBER - 1]->CreateChild<Urho3D::CheckBox>();
		checks[i]->SetStyle("LeftMenuCheckBox", style);
		checks[i]->SetVar("Num", i);
		checks[i]->AddChild(sprite);
		SubscribeToEvent(checks[i], Urho3D::E_CLICK, URHO3D_HANDLER(MenuPanel, ChengeModeButton));
	}
	auto texture = Game::getCache()->GetResource<Urho3D::Texture2D
	>("textures/hud/icon/lm/lm3.png");
	MySprite* sprite = createSprite(texture, style, "LeftMenuSmallSprite");
	nextButton = rows[LEFT_MENU_ROWS_NUMBER - 1]->CreateChild<Urho3D::Button>();
	nextButton->SetStyle("LeftMenuIcon", style);
	nextButton->AddChild(sprite);

	int k = 0;
	for (int i = 0; i < LEFT_MENU_ROWS_NUMBER - 1; ++i) {
		for (int j = 0; j < LEFT_MENU_BUTTON_PER_ROW; ++j) {
			sprites[k] = createEmptySprite(style, "LeftMenuSprite");
			buttons[k] = simpleButton(rows[i], sprites[k], style, "LeftMenuBigIcon");
			hudElements.push_back(new HudData(buttons[k]));
			hudElements[k]->setId(-1, LeftMenuAction::NONE);

			buttons[k]->SetVar("HudElement", hudElements[k]);
			k++;
		}
	}

}

void MenuPanel::setChecks(int val) {
	for (auto check : checks) {
		check->SetChecked(false);
		check->SetVisible(true);
	}
	checks[val]->SetChecked(true);
}

void MenuPanel::ChengeModeButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::CheckBox*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	int val = element->GetVar("Num").GetInt();
	LeftMenuSubMode newSubMode = static_cast<LeftMenuSubMode>(val);
	if (newSubMode != subMode) {
		subMode = newSubMode;
		updateButtons(lastSelectedInfo);
	}
}

void MenuPanel::basicBuilding() {
	int size = Game::getDatabaseCache()->getBuildingSize();
	int nation = Game::getPlayersManager()->getActivePlayer()->getNation();
	int k = 0;
	for (int i = 0; i < size; ++i) {
		db_building* building = Game::getDatabaseCache()->getBuilding(i);
		if (building) {
			if (building->nation == nation) {
				setTexture(k, "textures/hud/icon/building/" + building->icon);

				hudElements[k]->setId(building->id, LeftMenuAction::BUILDING);
				k++;
			}
		}
	}
	resetButtons(k);
}

void MenuPanel::levelBuilding() {
	int size = Game::getDatabaseCache()->getBuildingSize();
	int nation = Game::getPlayersManager()->getActivePlayer()->getNation();
	int k = 0;
	for (int i = 0; i < size; ++i) {
		db_building* building = Game::getDatabaseCache()->getBuilding(i);
		int level = Game::getPlayersManager()->getActivePlayer()->getLevelForBuilding(i) + 1;
		auto opt = Game::getDatabaseCache()->getBuildingLevel(i, level);
		if (opt.has_value()) {
			if (building->nation == nation) {
				setTexture(k, "textures/hud/icon/building/levels/" + Urho3D::String(level) + "/" + building->icon);

				hudElements[k]->setId(building->id, LeftMenuAction::BUILDING_LEVEL);
				k++;
			}
		}
	}
	resetButtons(k);
}


std::unordered_set<int> MenuPanel::getUpgradePathInBuilding(std::vector<SelectedInfoType*>& infoTypes) {
	std::unordered_set<int> common = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	for (int i = 0; i < infoTypes.size(); ++i) {
		std::vector<Physical*>& data = infoTypes.at(i)->getData();
		if (!data.empty()) {
			auto possiblePaths = pathsIdsInbuilding(i);
			std::unordered_set<int> temp(common);
			for (const auto& id : temp) {
				if (possiblePaths.find(id) == possiblePaths.end()) {
					common.erase(id);
				}
			}
		}
	}

	return common;
}

std::unordered_set<int> MenuPanel::getUnitInBuilding(std::vector<SelectedInfoType*>& infoTypes) {
	std::unordered_set<int> common = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int nation = Game::getPlayersManager()->getActivePlayer()->getNation();
	for (int i = 0; i < infoTypes.size(); ++i) {
		std::vector<Physical*>& data = infoTypes.at(i)->getData();
		if (!data.empty()) {
			auto possibleUntis = unitsIdsForBuildingNation(nation, i);
			std::unordered_set<int> temp(common);
			for (const auto& id : temp) {
				if (possibleUntis.find(id) == possibleUntis.end()) {
					common.erase(id);
				}
			}
		}
	}

	return common;
}

void MenuPanel::basicUnit(SelectedInfo* selectedInfo) {
	auto toShow = getUnitInBuilding(selectedInfo->getSelectedTypes());
	int k = 0;

	for (auto id : toShow) {
		db_unit* unit = Game::getDatabaseCache()->getUnit(id);
		if (unit) {
			setTexture(k, "textures/hud/icon/unit/" + unit->icon);

			hudElements[k]->setId(unit->id, LeftMenuAction::UNIT);
			k++;
		}
	}
	resetButtons(k);
}

void MenuPanel::levelUnit(SelectedInfo* selectedInfo) {
	auto toShow = getUnitInBuilding(selectedInfo->getSelectedTypes());
	int k = 0;

	for (auto id : toShow) {
		db_unit* unit = Game::getDatabaseCache()->getUnit(id);
		int level = Game::getPlayersManager()->getActivePlayer()->getLevelForUnit(id) + 1;
		auto opt = Game::getDatabaseCache()->getUnitLevel(id, level);
		if (opt.has_value()) {
			setTexture(k, "textures/hud/icon/unit/levels/" + Urho3D::String(level) + "/" + unit->icon);

			hudElements[k]->setId(unit->id, LeftMenuAction::UNIT_LEVEL);
			k++;
		}
	}
	resetButtons(k);
}

void MenuPanel::upgradeUnit(SelectedInfo* selectedInfo) {
	auto toShow = getUpgradePathInBuilding(selectedInfo->getSelectedTypes());
	int k = 0;

	for (auto id : toShow) {
		int level = Game::getPlayersManager()->getActivePlayer()->getLevelForUnitUpgrade(id) + 1;
		auto opt = Game::getDatabaseCache()->getUnitUpgrade(id, level);

		if (opt.has_value()) {
			auto upgrade = opt.value();
			setTexture(k, "textures/hud/icon/unit/upgrades/" + upgrade->pathName + "/" + upgrade->name + ".png");

			hudElements[k]->setId(id, LeftMenuAction::UNIT_UPGRADE);
			k++;
		}
	}
	resetButtons(k);
}

void MenuPanel::setTexture(int k, Urho3D::String textureName) {
	auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>(textureName);
	setTextureToSprite(sprites[k], texture);

	buttons[k]->SetVisible(true);
}

std::unordered_set<int> MenuPanel::getOrderForUnit(std::vector<SelectedInfoType*>& infoTypes) {
	std::unordered_set<int> common = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	for (int i = 0; i < infoTypes.size(); ++i) {
		std::vector<Physical*>& data = infoTypes.at(i)->getData();
		if (!data.empty()) {
			auto orders = Game::getDatabaseCache()->getOrdersForUnit(i);
			std::unordered_set<int> common2;
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
	return common;
}

void MenuPanel::basicOrder(SelectedInfo* selectedInfo) {
	std::unordered_set<int> toShow = getOrderForUnit(selectedInfo->getSelectedTypes());

	int k = 0;
	for (auto id : toShow) {
		db_order* order = Game::getDatabaseCache()->getOrder(id);
		if (order) {
			setTexture(k, "textures/hud/icon/orders/" + order->icon);

			hudElements[k]->setId(order->id, LeftMenuAction::ORDER);
			k++;
		}
	}
	resetButtons(k);
}

void MenuPanel::formationOrder() {
	int k = 0;
	setTexture(k, "textures/hud/icon/formation/none.png");

	hudElements[k]->setId(0, LeftMenuAction::FORMATION);
	k++;
	setTexture(k, "textures/hud/icon/formation/square.png");

	hudElements[k]->setId(1, LeftMenuAction::FORMATION);
	k++;
	resetButtons(k);
}

void MenuPanel::resetButtons(int from) {
	for (int i = from; i < LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1); ++i) {
		setTextureToSprite(sprites[i], nullptr);
		buttons[i]->SetVisible(false);
	}
}

void MenuPanel::updateButtons(SelectedInfo* selectedInfo) {
	setChecks(subMode);
	switch (mode) {

	case LeftMenuMode::BUILDING:
		buildingMenu();
		break;
	case LeftMenuMode::UNIT:
		unitMenu(selectedInfo);
		break;
	case LeftMenuMode::ORDER:
		orderMenu(selectedInfo);
		break;
	default: ;
	}
}

void MenuPanel::unitMenu(SelectedInfo* selectedInfo) {
	switch (subMode) {
	case BASIC:
		basicUnit(selectedInfo);
		break;
	case LEVEL:
		levelUnit(selectedInfo);
		break;
	case UPGRADE:
		upgradeUnit(selectedInfo);
		break;
	default: ;
	}
}

void MenuPanel::buildingMenu() {
	switch (subMode) {
	case BASIC:
		basicBuilding();
		break;
	case LEVEL:
		levelBuilding();
		break;
	case UPGRADE: break;
	default: ;
	}
}

void MenuPanel::orderMenu(SelectedInfo* selectedInfo) {
	switch (subMode) {
	case BASIC:

		basicOrder(selectedInfo);
		break;
	case LEVEL:
		formationOrder();
		break;
	case UPGRADE: break;
	default: ;
	}
}

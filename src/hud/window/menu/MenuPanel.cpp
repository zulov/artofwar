#include "MenuPanel.h"
#include "GameState.h"
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "database/DatabaseCache.h"
#include "hud/HudData.h"
#include "hud/MySprite.h"
#include "hud/UiUtils.h"
#include "hud/window/in_game_menu/middle/AbstractMiddlePanel.h"
#include "info/LeftMenuInfoPanel.h"
#include "objects/building/BuildingUtils.h"
#include "objects/MenuAction.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "utils.h"
#include "objects/resource/ResourceOrder.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/UIEvents.h>
#include <unordered_set>


MenuPanel::MenuPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "LeftMenuWindow",
                                                                    {GameState::RUNNING, GameState::PAUSE}) {
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
		updateMode(mode);
		subMode = LeftMenuSubMode::BASIC;
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

void MenuPanel::setCheckVisibility(std::initializer_list<bool> active) {
	int k = 0;
	for (auto a : active) {
		checks[k++]->SetVisible(a);
	}
}

void MenuPanel::updateMode(LeftMenuMode mode) {
	switch (mode) {
	case LeftMenuMode::BUILDING:
		return setCheckVisibility({true, true, false});
	case LeftMenuMode::UNIT:
		return setCheckVisibility({true, true, true});
	case LeftMenuMode::ORDER:
		return setCheckVisibility({true, true, false});
	case LeftMenuMode::RESOURCE:
		return setCheckVisibility({true, false, false});
	}
}

void MenuPanel::createBody() {
	infoPanel = new LeftMenuInfoPanel(style);
	infoPanel->createWindow();
	infoPanel->setVisible(false);

	mock = createElement<Urho3D::UIElement>(window, style, "LeftMenuMock");

	for (auto& row : rows) {
		row = createElement<Urho3D::UIElement>(mock, style, "LeftMenuListRow");
	}
	for (int i = 0; i < LEFT_MENU_CHECKS_NUMBER; ++i) {
		auto texture = Game::getCache()->GetResource<Urho3D::Texture2D
		>("textures/hud/icon/lm/lm" + Urho3D::String(i) + ".png");

		checks[i] = createElement<Urho3D::CheckBox>(rows[LEFT_MENU_ROWS_NUMBER - 1], style, "LeftMenuCheckBox");
		checks[i]->SetVar("Num", i);
		createSprite(checks[i], texture, style, "LeftMenuSmallSprite");
		SubscribeToEvent(checks[i], Urho3D::E_CLICK, URHO3D_HANDLER(MenuPanel, ChengeModeButton));
	}
	const auto texture = Game::getCache()->GetResource<Urho3D::Texture2D
	>("textures/hud/icon/lm/lm3.png");
	nextButton = createElement<Urho3D::Button>(rows[LEFT_MENU_ROWS_NUMBER - 1], style, "LeftMenuIcon");
	createSprite(nextButton, texture, style, "LeftMenuSmallSprite");

	int k = 0;
	for (int i = 0; i < LEFT_MENU_ROWS_NUMBER - 1; ++i) {
		for (int j = 0; j < LEFT_MENU_BUTTON_PER_ROW; ++j) {
			buttons[k] = createElement<Urho3D::Button>(rows[i], style, "LeftMenuBigIcon");
			sprites[k] = createElement<MySprite>(buttons[k], style, "LeftMenuSprite");
			hudElements.push_back(new HudData(buttons[k]));
			hudElements[k]->set(-1, MenuAction::NONE, "");

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

void MenuPanel::ChengeModeButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::CheckBox*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());

	auto newSubMode = static_cast<LeftMenuSubMode>(element->GetVar("Num").GetInt());
	if (newSubMode != subMode) {
		subMode = newSubMode;
		updateButtons(lastSelectedInfo);
	}
}

void MenuPanel::setNextElement(int& k, Urho3D::String texture, int id, MenuAction menuAction, Urho3D::String text) {
	setTextureToSprite(sprites[k], Game::getCache()->GetResource<Urho3D::Texture2D>(texture));

	buttons[k]->SetVisible(true);

	hudElements[k]->set(id, menuAction, text);
	k++;
}

void MenuPanel::basicBuilding() {
	int size = Game::getDatabaseCache()->getBuildingSize();
	int nation = Game::getPlayersManager()->getActivePlayer()->getNation();
	int k = 0;
	for (int i = 0; i < size; ++i) {
		db_building* building = Game::getDatabaseCache()->getBuilding(i);
		if (building && building->nation == nation) {
			setNextElement(k, "textures/hud/icon/building/" + building->icon, building->id, MenuAction::BUILDING, "");
		}
	}
	resetButtons(k);
}

void MenuPanel::levelBuilding() {
	int size = Game::getDatabaseCache()->getBuildingSize();
	int nation = Game::getPlayersManager()->getActivePlayer()->getNation();
	int k = 0;
	for (int i = 0; i < size; ++i) {
		auto building = Game::getDatabaseCache()->getBuilding(i);
		int level = Game::getPlayersManager()->getActivePlayer()->getLevelForBuilding(i) + 1;
		auto opt = Game::getDatabaseCache()->getBuildingLevel(i, level);
		if (opt.has_value()) {
			if (building->nation == nation) {
				setNextElement(k, "textures/hud/icon/building/levels/" + Urho3D::String(level) + "/" + building->icon,
				               building->id, MenuAction::BUILDING_LEVEL, "");
			}
		}
	}
	resetButtons(k);
}

std::unordered_set<int> MenuPanel::getUpgradePathInBuilding(std::vector<SelectedInfoType*>& infoTypes) {
	std::unordered_set<int> common = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	for (int i = 0; i < infoTypes.size(); ++i) {
		if (!infoTypes.at(i)->getData().empty()) {
			removefromCommon(common, pathsIdsInbuilding(i));
		}
	}

	return common;
}

std::unordered_set<int> MenuPanel::getUnitInBuilding(std::vector<SelectedInfoType*>& infoTypes) {
	std::unordered_set<int> common = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int nation = Game::getPlayersManager()->getActivePlayer()->getNation();
	for (int i = 0; i < infoTypes.size(); ++i) {
		if (!infoTypes.at(i)->getData().empty()) {
			removefromCommon(common, unitsIdsForBuildingNation(nation, i));
		}
	}

	return common;
}

void MenuPanel::removefromCommon(std::unordered_set<int>& common, std::unordered_set<int>& set) {
	std::unordered_set<int> temp(common);
	for (const auto& id : temp) {
		if (set.find(id) == set.end()) {
			common.erase(id);
		}
	}
}

void MenuPanel::basicUnit(SelectedInfo* selectedInfo) {
	int k = 0;
	for (auto id : getUnitInBuilding(selectedInfo->getSelectedTypes())) {
		db_unit* unit = Game::getDatabaseCache()->getUnit(id);
		if (unit) {
			setNextElement(k, "textures/hud/icon/unit/" + unit->icon, unit->id, MenuAction::UNIT, "");
		}
	}
	resetButtons(k);
}

void MenuPanel::levelUnit(SelectedInfo* selectedInfo) {
	int k = 0;
	for (auto id : getUnitInBuilding(selectedInfo->getSelectedTypes())) {
		int level = Game::getPlayersManager()->getActivePlayer()->getLevelForUnit(id) + 1;
		auto opt = Game::getDatabaseCache()->getUnitLevel(id, level);
		if (opt.has_value()) {
			db_unit* unit = Game::getDatabaseCache()->getUnit(id);
			setNextElement(k, "textures/hud/icon/unit/levels/" + Urho3D::String(level) + "/" + unit->icon, unit->id,
			               MenuAction::UNIT_LEVEL, "");
		}
	}
	resetButtons(k);
}

void MenuPanel::upgradeUnit(SelectedInfo* selectedInfo) {
	int k = 0;

	for (auto id : getUpgradePathInBuilding(selectedInfo->getSelectedTypes())) {
		int level = Game::getPlayersManager()->getActivePlayer()->getLevelForUnitUpgrade(id) + 1;
		auto opt = Game::getDatabaseCache()->getUnitUpgrade(id, level);

		if (opt.has_value()) {
			auto upgrade = opt.value();
			setNextElement(k, "textures/hud/icon/unit/upgrades/" + upgrade->pathName + "/" + upgrade->name + ".png", id,
			               MenuAction::UNIT_UPGRADE, "");
		}
	}
	resetButtons(k);
}

std::unordered_set<int> MenuPanel::getOrderForUnit(std::vector<SelectedInfoType*>& infoTypes) {
	std::unordered_set<int> common = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	for (int i = 0; i < infoTypes.size(); ++i) {
		if (!infoTypes.at(i)->getData().empty()) {
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
	int k = 0;
	for (auto id : getOrderForUnit(selectedInfo->getSelectedTypes())) {
		db_order* order = Game::getDatabaseCache()->getOrder(id);
		if (order) {
			setNextElement(k, "textures/hud/icon/orders/" + order->icon, order->id, MenuAction::ORDER, "");
		}
	}
	resetButtons(k);
}

void MenuPanel::formationOrder() {
	int k = 0;
	auto l10n = Game::getLocalization();
	setNextElement(k, "textures/hud/icon/formation/none.png", 0, MenuAction::FORMATION, l10n->Get("form_none"));
	setNextElement(k, "textures/hud/icon/formation/square.png", 1, MenuAction::FORMATION, l10n->Get("form_square"));

	resetButtons(k);
}

void MenuPanel::resetButtons(int from) {
	for (int i = from; i < LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1); ++i) {
		setTextureToSprite(sprites[i], nullptr);
		buttons[i]->SetVisible(false);
	}
}

void MenuPanel::updateButtons(SelectedInfo* selectedInfo) {
	setChecks(static_cast<int>(subMode));
	switch (mode) {
	case LeftMenuMode::BUILDING:
		return buildingMenu();
	case LeftMenuMode::UNIT:
		return unitMenu(selectedInfo);
	case LeftMenuMode::ORDER:
		return orderMenu(selectedInfo);
	case LeftMenuMode::RESOURCE:
		return resourceMenu(selectedInfo);
	default: ;
	}
}

void MenuPanel::unitMenu(SelectedInfo* selectedInfo) {
	switch (subMode) {
	case LeftMenuSubMode::BASIC:
		return basicUnit(selectedInfo);
	case LeftMenuSubMode::LEVEL:
		return levelUnit(selectedInfo);
	case LeftMenuSubMode::UPGRADE:
		return upgradeUnit(selectedInfo);
	}
}

void MenuPanel::buildingMenu() {
	switch (subMode) {
	case LeftMenuSubMode::BASIC:
		return basicBuilding();
	case LeftMenuSubMode::LEVEL:
		return levelBuilding();
	}
}

void MenuPanel::orderMenu(SelectedInfo* selectedInfo) {
	switch (subMode) {
	case LeftMenuSubMode::BASIC:
		return basicOrder(selectedInfo);
	case LeftMenuSubMode::LEVEL:
		return formationOrder();
	}
}

void MenuPanel::resourceMenu(SelectedInfo* selectedInfo) {
	switch (subMode) {
	case LeftMenuSubMode::BASIC:
		return basicResource(selectedInfo);
	}
}

void MenuPanel::basicResource(SelectedInfo* selectedInfo) {
	int k = 0;
	auto l10n = Game::getLocalization();

	setNextElement(k, "textures/hud/icon/resource_action/get_worker.png", int(ResourceOrder::COLLECT),
	               MenuAction::RESOURCE,
	               l10n->Get("res_act_get_worker"));
	setNextElement(k, "textures/hud/icon/resource_action/remove_workers.png", int(ResourceOrder::COLLECT_CANCEL),
	               MenuAction::RESOURCE,
	               l10n->Get("res_act_cancel_worker"));

	resetButtons(k);
}

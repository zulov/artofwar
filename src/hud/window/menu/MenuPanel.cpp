#include "MenuPanel.h"
#include <iostream>
#include <utility>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/UIEvents.h>
#include "DeleteUtils.h"
#include "GameState.h"
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "database/DatabaseCache.h"
#include "hud/HudData.h"
#include "hud/MySprite.h"
#include "hud/UiUtils.h"
#include "hud/window/in_game_menu/middle/AbstractMiddlePanel.h"
#include "info/LeftMenuInfoPanel.h"
#include "objects/ActionType.h"
#include "player/Player.h"
#include "player/PlayersManager.h"

#include "commands/action/ResourceActionType.h"
#include "math/VectorUtils.h"


MenuPanel::MenuPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "LeftMenuWindow",
                                                                    {GameState::RUNNING, GameState::PAUSE}) {
}


MenuPanel::~MenuPanel() {
	delete infoPanel;
	clear_vector(hudElements);
}

void MenuPanel::removeHoverInfo() const {
	infoPanel->removeHoverInfo();
}

void MenuPanel::refresh(LeftMenuMode _mode, SelectedInfo* selectedInfo) {//TODO performance rzadniej odświeżać
	lastSelectedInfo = selectedInfo;
	if (mode != _mode) {
		mode = _mode;
		updateMode(mode);
		subMode = LeftMenuSubMode::BASIC;
	}
	updateButtons(lastSelectedInfo);
}

void MenuPanel::setHoverInfo(HudData* hudData) const {
	infoPanel->setHoverInfo(hudData);
}

void MenuPanel::updateSelected(SelectedInfo* selectedInfo) const {
	infoPanel->updateSelected(selectedInfo);
}

void MenuPanel::setVisible(bool enable) {
	AbstractWindowPanel::setVisible(enable);
	if (!enable) {
		removeHoverInfo();
	}
}

std::vector<HudData*>& MenuPanel::getButtons() {
	return hudElements;
}

void MenuPanel::clearSelected() const {
	infoPanel->clearSelected();
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
	removeHoverInfo();

	mock = createElement<Urho3D::UIElement>(window, style, "LeftMenuMock");

	for (auto& row : rows) {
		row = createElement<Urho3D::UIElement>(mock, style, "LeftMenuListRow");
	}
	for (int i = 0; i < LEFT_MENU_CHECKS_NUMBER; ++i) {
		const auto texture = Game::getCache()->GetResource<Urho3D::Texture2D
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
			hudElements[k]->set(-1, ActionType::NONE, "");

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

void MenuPanel::setNext(int& k, const Urho3D::String& texture, int id, ActionType menuAction, Urho3D::String text) {
	setTextureToSprite(sprites[k], Game::getCache()->GetResource<Urho3D::Texture2D>(texture));

	buttons[k]->SetVisible(true);

	hudElements[k]->set(id, menuAction, std::move(text));
	k++;
}

void MenuPanel::basicBuilding() {
	int nation = Game::getPlayersMan()->getActivePlayer()->getNation();
	int k = 0;
	for (auto building : Game::getDatabase()->getNation(nation)->buildings) {
		setNext(k, "textures/hud/icon/building/" + building->icon, building->id, ActionType::BUILDING_CREATE, "");
	}
	resetRestButtons(k);
}

void MenuPanel::levelBuilding() {
	int nation = Game::getPlayersMan()->getActivePlayer()->getNation();
	int k = 0;
	for (auto building : Game::getDatabase()->getNation(nation)->buildings) {
		auto opt = Game::getPlayersMan()->getActivePlayer()->getNextLevelForBuilding(building->id);
		if (opt.has_value()) {
			setNext(k, "textures/hud/icon/building/levels/" + Urho3D::String(opt.value()->level) + "/" + building->icon,
			        building->id, ActionType::BUILDING_LEVEL, "");
		}
	}
	resetRestButtons(k);
}

std::vector<short> MenuPanel::getUnitInBuilding(SelectedInfo* selectedInfo) {
	if (selectedInfo->getAllNumber() <= 0) { return {}; }

	auto& infoTypes = selectedInfo->getSelectedTypes();
	int nation = Game::getPlayersMan()->getActivePlayer()->getNation();
	std::vector<std::vector<short>*> ids;
	for (int i = 0; i < infoTypes.size(); ++i) {
		if (!infoTypes.at(i)->getData().empty()) {
			ids.push_back(Game::getPlayersMan()->getActivePlayer()->getLevelForBuilding(i)->unitsPerNationIds[nation]);
		}
	}
	return intersection(ids);
}

void MenuPanel::basicUnit(SelectedInfo* selectedInfo) {
	int k = 0;
	for (auto id : getUnitInBuilding(selectedInfo)) {
		db_unit* unit = Game::getDatabase()->getUnit(id);
		if (unit) {
			setNext(k, "textures/hud/icon/unit/" + unit->icon, unit->id, ActionType::UNIT_CREATE, "");
		}
	}
	resetRestButtons(k);
}

void MenuPanel::levelUnit(SelectedInfo* selectedInfo) {
	int k = 0;
	for (auto id : getUnitInBuilding(selectedInfo)) {
		auto opt = Game::getPlayersMan()->getActivePlayer()->getNextLevelForUnit(id);
		if (opt.has_value()) {
			db_unit* unit = Game::getDatabase()->getUnit(id);
			setNext(k, "textures/hud/icon/unit/levels/" + Urho3D::String(opt.value()->level) + "/" + unit->icon,
			        unit->id,
			        ActionType::UNIT_LEVEL, "");
		}
	}
	resetRestButtons(k);
}

std::vector<short> MenuPanel::getOrderForUnit(SelectedInfo* selectedInfo) {
	if (selectedInfo->getAllNumber() <= 0) { return {}; }

	auto& infoTypes = selectedInfo->getSelectedTypes();

	std::vector<std::vector<short>*> ids;
	for (int i = 0; i < infoTypes.size(); ++i) {
		if (!infoTypes.at(i)->getData().empty()) {
			ids.push_back(&Game::getDatabase()->getUnit(i)->ordersIds);
		}
	}
	return intersection(ids);
}


void MenuPanel::basicOrder(SelectedInfo* selectedInfo) {
	int k = 0;

	for (auto id : getOrderForUnit(selectedInfo)) {
		db_order* order = Game::getDatabase()->getOrder(id);
		if (order) {
			setNext(k, "textures/hud/icon/orders/" + order->icon, order->id, ActionType::ORDER, "");
		}
	}
	resetRestButtons(k);
}

void MenuPanel::formationOrder() {
	int k = 0;
	auto l10n = Game::getLocalization();
	setNext(k, "textures/hud/icon/formation/none.png", 0, ActionType::FORMATION, l10n->Get("form_none"));
	setNext(k, "textures/hud/icon/formation/square.png", 1, ActionType::FORMATION, l10n->Get("form_square"));

	resetRestButtons(k);
}

void MenuPanel::resetRestButtons(int from) {
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

	setNext(k, "textures/hud/icon/resource_action/get_worker.png", int(ResourceActionType::COLLECT),
	        ActionType::RESOURCE,
	        l10n->Get("res_act_get_worker"));
	setNext(k, "textures/hud/icon/resource_action/remove_workers.png", int(ResourceActionType::CANCEL),
	        ActionType::RESOURCE,
	        l10n->Get("res_act_cancel_worker"));

	resetRestButtons(k);
}

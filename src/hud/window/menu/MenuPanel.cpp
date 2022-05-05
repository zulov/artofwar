#include "MenuPanel.h"

#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include "control/SelectedInfo.h"
#include "control/SelectedInfoType.h"
#include "database/DatabaseCache.h"
#include "hud/HudData.h"
#include "hud/UiUtils.h"
#include "info/LeftMenuInfoPanel.h"
#include "objects/ActionType.h"
#include "player/Player.h"
#include "player/PlayersManager.h"
#include "commands/action/ResourceActionType.h"
#include "database/db_other_struct.h"
#include "math/VectorUtils.h"

static constexpr const char* ICONS_PATH = "textures/hud/icon/";

MenuPanel::MenuPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style) : EventPanel(root, _style, "LeftMenuWindow",
	{GameState::RUNNING, GameState::PAUSE}) {
}

MenuPanel::~MenuPanel() {
	delete infoPanel;
	clear_vector(hudElements);
}

void MenuPanel::removeHoverInfo() const {
	infoPanel->removeHoverInfo();
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

void MenuPanel::setHoverInfo(HudData* hudData) const {
	infoPanel->setHoverInfo(hudData);
}

void MenuPanel::updateSelected(SelectedInfo* selectedInfo) const {
	infoPanel->updateSelected(selectedInfo);
}

void MenuPanel::setVisible(bool enable) {
	SimplePanel::setVisible(enable);
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
	infoPanel = new LeftMenuInfoPanel(root, style);
	infoPanel->createWindow();
	removeHoverInfo();

	const auto mock = createElement<Urho3D::UIElement>(window, style, "LeftMenuMock");

	for (auto& row : rows) {
		row = createElement<Urho3D::UIElement>(mock, style, "LeftMenuListRow");
	}
	for (int i = 0; i < LEFT_MENU_CHECKS_NUMBER; ++i) {
		const auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>(
			"textures/hud/icon/lm/lm" + Urho3D::String(i) + ".png");

		checks[i] = createElement<Urho3D::CheckBox>(rows[LEFT_MENU_ROWS_NUMBER - 1], style, "LeftMenuCheckBox");
		checks[i]->SetVar("Num", i);
		createSprite(checks[i], texture, style, "LeftMenuSmallSprite");
		SubscribeToEvent(checks[i], Urho3D::E_CLICK, URHO3D_HANDLER(MenuPanel, ChangeModeButton));
	}
	const auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/lm/lm3.png");
	const auto nextButton = createElement<Urho3D::Button>(rows[LEFT_MENU_ROWS_NUMBER - 1], style, "LeftMenuIcon");
	createSprite(nextButton, texture, style, "LeftMenuSmallSprite");

	int k = 0;
	for (int i = 0; i < LEFT_MENU_ROWS_NUMBER - 1; ++i) {
		const auto row = rows[i];
		for (int j = 0; j < LEFT_MENU_BUTTON_PER_ROW; ++j) {
			buttons[k] = createElement<Urho3D::Button>(row, style, "LeftMenuBigIcon");
			sprites[k] = createElement<MySprite>(buttons[k], style, "LeftMenuSprite");
			hudElements.push_back(new HudData(buttons[k]));
			hudElements[k]->set(-1, ActionType::NONE, "");

			buttons[k]->SetVar("HudElement", hudElements[k]);
			k++;
		}
	}
}

void MenuPanel::setChecks(int val) {
	for (const auto check : checks) {
		check->SetChecked(false);
	}
	checks[val]->SetChecked(true);
}

void MenuPanel::ChangeModeButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::CheckBox*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());

	auto newSubMode = static_cast<LeftMenuSubMode>(element->GetVar("Num").GetInt());
	if (newSubMode != subMode) {
		subMode = newSubMode;
		updateButtons(lastSelectedInfo);
	}
}

void MenuPanel::setNext(int& k, const Urho3D::String& texture, int id, ActionType menuAction,
                        Urho3D::String text) const {
	setTextureToSprite(sprites[k], Game::getCache()->GetResource<Urho3D::Texture2D>(ICONS_PATH + texture));

	buttons[k]->SetVisible(true);

	hudElements[k]->set(id, menuAction, std::move(text));
	k++;
}

void MenuPanel::basicBuilding(char site) {
	int nation = Game::getPlayersMan()->getActivePlayer()->getNation();
	int k = 0;
	for (const auto building : Game::getDatabase()->getNation(nation)->buildings) {
		if (k >= LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1)) {
			continue;
		}
		setNext(k, "building/" + building->icon, building->id, ActionType::BUILDING_CREATE, "");

	}
	resetRestButtons(k);
}

void MenuPanel::levelBuilding(char site) {
	int nation = Game::getPlayersMan()->getActivePlayer()->getNation();
	int k = 0;
	for (auto building : Game::getDatabase()->getNation(nation)->buildings) {
		auto opt = Game::getPlayersMan()->getActivePlayer()->getNextLevelForBuilding(building->id);
		if (opt.has_value()) {
			setNext(k, "building/levels/" + Urho3D::String(opt.value()->level) + "/" + building->icon,
			        building->id, ActionType::BUILDING_LEVEL, "");
		}
	}
	resetRestButtons(k);
}

std::vector<unsigned char> MenuPanel::getUnitInBuilding(SelectedInfo* selectedInfo) const {
	if (selectedInfo->getAllNumber() <= 0) { return {}; }

	auto& infoTypes = selectedInfo->getSelectedTypes();
	int nation = Game::getPlayersMan()->getActivePlayer()->getNation();
	std::vector<std::vector<unsigned char>*> ids;
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
			setNext(k, "unit/levels/" + Urho3D::String(opt.value()->level) + "/" + unit->icon,
			        unit->id, ActionType::UNIT_LEVEL, "");
		}
	}
	resetRestButtons(k);
}

std::vector<unsigned char> MenuPanel::getOrderForUnit(SelectedInfo* selectedInfo) const {
	if (selectedInfo->getAllNumber() <= 0) { return {}; }

	auto& infoTypes = selectedInfo->getSelectedTypes();

	std::vector<std::vector<unsigned char>*> ids;
	for (int i = 0; i < infoTypes.size(); ++i) {
		const auto info = infoTypes.at(i);
		if (!info->getData().empty()) {
			const auto dbUnit = Game::getDatabase()->getUnit(i);
			ids.push_back(&dbUnit->ordersIds);
		}
	}
	return intersection(ids);
}


void MenuPanel::basicOrder(SelectedInfo* selectedInfo, char page) {
	int k = 0;

	for (auto id : getOrderForUnit(selectedInfo)) {
		db_order* order = Game::getDatabase()->getOrder(id);
		if (order) {
			setNext(k, "textures/hud/icon/orders/" + order->icon, order->id, ActionType::ORDER, "");
		}
	}
	resetRestButtons(k);
}

void MenuPanel::formationOrder(char page) {
	int k = 0;
	auto l10n = Game::getLocalization();
	setNext(k, "formation/none.png", 0, ActionType::FORMATION, l10n->Get("form_none"));
	setNext(k, "formation/square.png", 1, ActionType::FORMATION, l10n->Get("form_square"));

	resetRestButtons(k);
}

void MenuPanel::resetRestButtons(int from) {
	for (int i = from; i < LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1); ++i) {
		setTextureToSprite(sprites[i], nullptr);
		buttons[i]->SetVisible(false);
	}
}

void MenuPanel::updateButtons(SelectedInfo* selectedInfo, char page) {
	setChecks(static_cast<int>(subMode));
	switch (mode) {
	case LeftMenuMode::BUILDING:
		return buildingMenu(page);
	case LeftMenuMode::UNIT:
		return unitMenu(selectedInfo, page);
	case LeftMenuMode::ORDER:
		return orderMenu(selectedInfo, page);
	case LeftMenuMode::RESOURCE:
		return resourceMenu(selectedInfo, page);
	default: ;
	}
}

void MenuPanel::unitMenu(SelectedInfo* selectedInfo, char page) {
	switch (subMode) {
	case LeftMenuSubMode::BASIC:
		return basicUnit(selectedInfo);
	case LeftMenuSubMode::LEVEL:
		return levelUnit(selectedInfo);
	}
}

void MenuPanel::buildingMenu(char page) {
	switch (subMode) {
	case LeftMenuSubMode::BASIC:
		return basicBuilding(page);
	case LeftMenuSubMode::LEVEL:
		return levelBuilding(page);
	}
}

void MenuPanel::orderMenu(SelectedInfo* selectedInfo, char page) {
	switch (subMode) {
	case LeftMenuSubMode::BASIC:
		return basicOrder(selectedInfo, page);
	case LeftMenuSubMode::LEVEL:
		return formationOrder(page);
	}
}

void MenuPanel::resourceMenu(SelectedInfo* selectedInfo, char page) {
	switch (subMode) {
	case LeftMenuSubMode::BASIC:
		return basicResource(selectedInfo, page);
	}
}

void MenuPanel::basicResource(SelectedInfo* selectedInfo, char page) {
	int k = 0;
	auto l10n = Game::getLocalization();

	setNext(k, "resource_action/get_worker.png", int(ResourceActionType::COLLECT),
	        ActionType::RESOURCE, l10n->Get("res_act_get_worker"));
	setNext(k, "resource_action/remove_workers.png", int(ResourceActionType::CANCEL),
	        ActionType::RESOURCE, l10n->Get("res_act_cancel_worker"));

	resetRestButtons(k);
}

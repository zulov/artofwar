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
#include "objects/unit/order/UnitConst.h"

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
		page = 0;
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
	Urho3D::UIElement* rows[ROWS_NUMBER];
	for (auto& row : rows) {
		row = createElement<Urho3D::UIElement>(mock, style, "LeftMenuListRow");
	}
	for (int i = 0; i < CHECKS_NUMBER; ++i) {
		const auto texture = getTexture("textures/hud/icon/lm/lm" + Urho3D::String(i) + ".png");

		checks[i] = createElement<Urho3D::CheckBox>(rows[ROWS_NUMBER - 1], style, "LeftMenuCheckBox");
		checks[i]->SetVar("Num", i);
		createSprite(checks[i], texture, style, "LeftMenuSmallSprite");
		SubscribeToEvent(checks[i], Urho3D::E_CLICK, URHO3D_HANDLER(MenuPanel, ChangeModeButton));
	}
	const auto texture = getTexture("textures/hud/icon/lm/lm3.png");
	const auto nextButton = createElement<Urho3D::Button>(rows[ROWS_NUMBER - 1], style, "LeftMenuIcon");
	SubscribeToEvent(nextButton, Urho3D::E_CLICK, URHO3D_HANDLER(MenuPanel, NextPage));
	createSprite(nextButton, texture, style, "LeftMenuSmallSprite");

	int k = 0;
	for (int i = 0; i < ROWS_NUMBER - 1; ++i) {
		const auto row = rows[i];
		for (int j = 0; j < BUTTONS_PER_ROW; ++j) {
			buttons[k] = createElement<Urho3D::Button>(row, style, "LeftMenuBigIcon");
			sprites[k] = createElement<MySprite>(buttons[k], style, "LeftMenuSprite");
			hudElements.push_back(new HudData(buttons[k]));
			hudElements[k]->set(-1, ActionType::NONE, "");

			buttons[k]->SetVar("HudElement", hudElements[k]);
			k++;
		}
	}
}

void MenuPanel::setChecks(char val) {
	for (const auto check : checks) {
		check->SetChecked(false);
	}
	checks[val]->SetChecked(true);
}

void MenuPanel::NextPage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	page = (page + 1) % maxPage;
	updateButtons(lastSelectedInfo);
}

void MenuPanel::ChangeModeButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::CheckBox*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());

	auto newSubMode = static_cast<LeftMenuSubMode>(element->GetVar("Num").GetInt());
	if (newSubMode != subMode) {
		subMode = newSubMode;
		page = 0;
		updateButtons(lastSelectedInfo);
	}
}

void MenuPanel::setNext(int& k, const Urho3D::String& texture, short id, ActionType menuAction,
                        Urho3D::String text) const {
	setTextureToSprite(sprites[k], Game::getCache()->GetResource<Urho3D::Texture2D>(ICONS_PATH + texture));

	buttons[k]->SetVisible(true);

	hudElements[k]->set(id, menuAction, std::move(text));
	++k;
}

void MenuPanel::basicBuilding() {
	const short nation = Game::getPlayersMan()->getActivePlayer()->getNation();

	setIcons(Game::getDatabase()->getNation(nation)->buildings, "building/", ActionType::BUILDING_CREATE);
}

void MenuPanel::levelBuilding() {
	int nation = Game::getPlayersMan()->getActivePlayer()->getNation();
	int k = 0;
	std::vector<db_building_level*> levels;
	const auto player = Game::getPlayersMan()->getActivePlayer();
	for (const auto building : Game::getDatabase()->getNation(nation)->buildings) {
		auto opt = player->getNextLevelForBuilding(building->id);
		if (opt.has_value()) {
			levels.push_back(opt.value());
		}
	}

	for (int i = page * BUTTONS_NUMBER; i < levels.size() && i < (page + 1) * BUTTONS_NUMBER; ++i) {
		const auto level = levels[i];
		const db_building* building = Game::getDatabase()->getBuilding(level->building);
		setNext(k, "building/levels/" + Urho3D::String((int)level->level) + "/" + building->icon,
		        building->id, ActionType::BUILDING_LEVEL);
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
	std::vector<db_with_icon*> units;
	for (auto id : getUnitInBuilding(selectedInfo)) {
		db_unit* unit = Game::getDatabase()->getUnit(id);
		if (unit) {
			units.push_back(unit);
		}
	}

	setIcons(units, "unit/", ActionType::UNIT_CREATE);
}

void MenuPanel::levelUnit(SelectedInfo* selectedInfo) {
	std::vector<db_unit_level*> levels;
	for (auto id : getUnitInBuilding(selectedInfo)) {
		auto opt = Game::getPlayersMan()->getActivePlayer()->getNextLevelForUnit(id);
		if (opt.has_value()) {
			levels.push_back(opt.value());
		}
	}
	int k = 0;
	maxPage = levels.size() / BUTTONS_NUMBER + 1;
	for (int i = page * BUTTONS_NUMBER; i < levels.size() && i < (page + 1) * BUTTONS_NUMBER; ++i) {
		auto level = levels[i];

		db_unit* unit = Game::getDatabase()->getUnit(level->unit);
		setNext(k, "unit/levels/" + Urho3D::String(level->level) + "/" + unit->icon, unit->id, ActionType::UNIT_LEVEL);
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

template <typename T>
void MenuPanel::setIcons(const std::vector<T*>& icons, Urho3D::String path, ActionType type) {
	int k = 0;
	maxPage = icons.size() / BUTTONS_NUMBER + 1;
	for (int i = page * BUTTONS_NUMBER; i < icons.size() && i < (page + 1) * BUTTONS_NUMBER; ++i) {
		auto icon = icons[i];

		setNext(k, path + icon->icon, icon->id, type);
	}
	resetRestButtons(k);
}


void MenuPanel::setOrderIcons(const std::vector<char>& ids, Urho3D::String path) {
	int k = 0;
	maxPage = ids.size() / BUTTONS_NUMBER + 1;
	for (int i = page * BUTTONS_NUMBER; i < ids.size() && i < (page + 1) * BUTTONS_NUMBER; ++i) {
		setNext(k, path + "orders/" + magic_enum::enum_name(static_cast<UnitOrderType>(i)).data() + ".png", i, ActionType::ORDER);
	}
	resetRestButtons(k);
}

void MenuPanel::basicOrder(SelectedInfo* selectedInfo) {
	std::vector<char> orders;
	for (auto id : getOrderForUnit(selectedInfo)) {
		orders.push_back(id);
	}

	setOrderIcons(orders, "orders/");
}

void MenuPanel::formationOrder() {
	int k = 0;
	const auto l10n = Game::getLocalization();
	setNext(k, "formation/none.png", 0, ActionType::FORMATION, l10n->Get("form_none"));
	setNext(k, "formation/square.png", 1, ActionType::FORMATION, l10n->Get("form_square"));
	maxPage = 1;

	resetRestButtons(k);
}

void MenuPanel::resetRestButtons(int from) const {
	for (int i = from; i < BUTTONS_PER_ROW * (ROWS_NUMBER - 1); ++i) {
		setTextureToSprite(sprites[i], nullptr);
		buttons[i]->SetVisible(false);
	}
}

void MenuPanel::updateButtons(SelectedInfo* selectedInfo) {
	setChecks(cast(subMode));
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
	const auto l10n = Game::getLocalization();

	setNext(k, "resource_action/get_worker.png", cast(ResourceActionType::COLLECT),
	        ActionType::RESOURCE, l10n->Get("res_act_get_worker"));
	setNext(k, "resource_action/remove_workers.png", cast(ResourceActionType::CANCEL),
	        ActionType::RESOURCE, l10n->Get("res_act_cancel_worker"));
	maxPage = 1;
	resetRestButtons(k);
}

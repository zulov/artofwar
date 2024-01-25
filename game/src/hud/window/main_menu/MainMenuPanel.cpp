#include "MainMenuPanel.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include "GameState.h"
#include "close//MainMenuClosePanel.h"
#include "help/MainMenuHelpPanel.h"
#include "hud/UiUtils.h"
#include "load/MainMenuLoadPanel.h"
#include "new_game/MainMenuNewGamePanel.h"
#include "settings/MainMenuSettingsPanel.h"


MainMenuPanel::MainMenuPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style): EventPanel(root, _style, "MainMenuWindow",
                                                                           {GameState::MENU_MAIN}) {}


MainMenuPanel::~MainMenuPanel() {
	clear_array(detailsPanels,MAIN_MENU_BUTTON_NUMBER);
}

void MainMenuPanel::action(short id) const {
	close();

	detailsPanels[id]->setVisible(true);
}

void MainMenuPanel::close() const {
	for (auto detailsPanel : detailsPanels) {
		detailsPanel->setVisible(false);
	}
}

void MainMenuPanel::setVisible(bool enable) {
	SimplePanel::setVisible(enable);
	close();
	background->SetVisible(enable);
}

Urho3D::Button* MainMenuPanel::getNewGameProceed() const {
	return dynamic_cast<MainMenuNewGamePanel*>(detailsPanels[0])->getProceed();
}

Urho3D::Button* MainMenuPanel::getLoadButton() const {
	return dynamic_cast<MainMenuLoadPanel*>(detailsPanels[1])->getLoadButton();
}

Urho3D::Button* MainMenuPanel::getCloseButton() const {
	return dynamic_cast<MainMenuClosePanel*>(detailsPanels[4])->getCloseButton();
}

void MainMenuPanel::HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	int id = element->GetVar("Num").GetInt();

	action(id);
}

void MainMenuPanel::createBody() {
	background = createElement<Urho3D::BorderImage>(Game::getUI()->GetRoot(), style, "MainMenuBackground");
	background->SetBringToBack(true);
	background->SetPriority(-1);
	window->SetPriority(1);

	auto l10n = Game::getLocalization();
	detailsPanels[0] = new MainMenuNewGamePanel(root, style, l10n->Get("menu_0"));
	detailsPanels[1] = new MainMenuLoadPanel(root, style, l10n->Get("menu_1"));
	detailsPanels[2] = new MainMenuSettingsPanel(root, style, l10n->Get("menu_2"));
	detailsPanels[3] = new MainMenuHelpPanel(root, style, l10n->Get("menu_3"));
	detailsPanels[4] = new MainMenuClosePanel(root, style, l10n->Get("menu_4"));
	for (auto detailsPanel : detailsPanels) {
		detailsPanel->createWindow();
		detailsPanel->setVisible(false);
	}

	for (int i = 0; i < MAIN_MENU_BUTTON_NUMBER; ++i) {
		auto texture2 = getTexture("textures/hud/icon/menu/menu_" + Urho3D::String(i) + ".png");

		auto button = createElement<Urho3D::Button>(window, style, "MainMenuButton");
		createSprite(button, texture2, style, "MainMenuSprite");

		addChildText(button, "MainMenuText", l10n->Get("menu_" + Urho3D::String(i)), style);

		button->SetVar("Num", i);
		SubscribeToEvent(button, Urho3D::E_CLICK, URHO3D_HANDLER(MainMenuPanel, HandleButtonClick));
	}
}

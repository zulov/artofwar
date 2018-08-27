#include "MainMenuPanel.h"
#include "GameState.h"
#include "close//MainMenuClosePanel.h"
#include "help/MainMenuHelpPanel.h"
#include "hud/MySprite.h"
#include "hud/UiUtils.h"
#include "load/MainMenuLoadPanel.h"
#include "new_game/MainMenuNewGamePanel.h"
#include "settings/MainMenuSettingsPanel.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>


MainMenuPanel::MainMenuPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style, "MainMenuWindow") {
	visibleAt[static_cast<char>(GameState::MENU_MAIN)] = true;
}


MainMenuPanel::~MainMenuPanel() {
	for (int i = 0; i < MAIN_MENU_BUTTON_NUMBER; ++i) {
		delete detailsPanels[i];
	}
	delete[] detailsPanels;
}

void MainMenuPanel::action(short id) {
	close();

	detailsPanels[id]->setVisible(true);
}

void MainMenuPanel::close() {
	for (int i = 0; i < MAIN_MENU_BUTTON_NUMBER; ++i) {
		detailsPanels[i]->setVisible(false);
	}
}

void MainMenuPanel::setVisible(bool enable) {
	AbstractWindowPanel::setVisible(enable);
	close();
	background->SetVisible(enable);
}

Urho3D::Button* MainMenuPanel::getNewGameProceed() {
	return static_cast<MainMenuNewGamePanel*>(detailsPanels[0])->getProceed();
}

Urho3D::Button* MainMenuPanel::getLoadButton() {
	return static_cast<MainMenuLoadPanel*>(detailsPanels[1])->getLoadButton();
}

Urho3D::Button* MainMenuPanel::getCloseButton() {
	return static_cast<MainMenuClosePanel*>(detailsPanels[4])->getCloseButton();
}

void MainMenuPanel::HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	int id = element->GetVar("Num").GetInt();

	action(id);
}

void MainMenuPanel::createBody() {
	background = Game::getUI()->GetRoot()->CreateChild<Urho3D::BorderImage>();
	background->SetStyle("MainMenuBackground", style);
	background->SetBringToBack(true);
	background->SetPriority(-1);
	window->SetPriority(1);

	detailsPanels = new MainMenuDetailsPanel*[MAIN_MENU_BUTTON_NUMBER];
	auto l10n = Game::getLocalization();
	detailsPanels[0] = new MainMenuNewGamePanel(style, l10n->Get("menu_0"));
	detailsPanels[1] = new MainMenuLoadPanel(style, l10n->Get("menu_1"));
	detailsPanels[2] = new MainMenuSettingsPanel(style, l10n->Get("menu_2"));
	detailsPanels[3] = new MainMenuHelpPanel(style, l10n->Get("menu_3"));
	detailsPanels[4] = new MainMenuClosePanel(style, l10n->Get("menu_4"));
	for (int i = 0; i < MAIN_MENU_BUTTON_NUMBER; ++i) {
		detailsPanels[i]->createWindow();
		detailsPanels[i]->setVisible(false);
	}

	for (int i = 0; i < MAIN_MENU_BUTTON_NUMBER; ++i) {
		auto texture2 = Game::getCache()->GetResource<Urho3D::Texture2D
		>("textures/hud/icon/menu/menu_" + Urho3D::String(i) + ".png");

		auto sprite2 = createSprite(texture2, style, "MainMenuSprite");
		auto button = simpleButton(sprite2, style, "MainMenuButton");
		auto text = button->CreateChild<Urho3D::Text>();
		auto msg = l10n->Get("menu_" + Urho3D::String(i));
		text->SetText(msg);
		text->SetStyle("MainMenuText", style);

		button->SetVar("Num", i);
		window->AddChild(button);
		SubscribeToEvent(button, Urho3D::E_CLICK, URHO3D_HANDLER(MainMenuPanel, HandleButtonClick));
	}
}

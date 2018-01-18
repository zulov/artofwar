#include "MainMenuPanel.h"
#include "hud/MySprite.h"
#include "hud/UiUtils.h"
#include "help/MainMenuHelpPanel.h"
#include "new_game/MainMenuNewGamePanel.h"
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include "GameState.h"

MainMenuPanel::MainMenuPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style) {
	styleName = "MainMenuWindow";
	visibleAt.insert(GameState::MENU_MAIN);
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
}

Urho3D::Button* MainMenuPanel::getNewGameProceed() {
	return static_cast<MainMenuNewGamePanel*>(detailsPanels[0])->getProceed();
}

void MainMenuPanel::HandleButtonClick(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	HudElement* hudElement = static_cast<HudElement *>(element->GetVar("HudElement").GetVoidPtr());

	action(hudElement->getId());
}

void MainMenuPanel::createBody() {
	detailsPanels = new MainMenuDetailsPanel*[MAIN_MENU_BUTTON_NUMBER];
	Localization* l10n = Game::get()->getLocalization();
	detailsPanels[0] = new MainMenuNewGamePanel(style, l10n->Get("menu_0"));
	detailsPanels[1] = new MainMenuDetailsPanel(style, l10n->Get("menu_1"));
	detailsPanels[2] = new MainMenuDetailsPanel(style, l10n->Get("menu_2"));
	detailsPanels[3] = new MainMenuHelpPanel(style, l10n->Get("menu_3"));
	detailsPanels[4] = new MainMenuDetailsPanel(style, l10n->Get("menu_4"));
	for (int i = 0; i < MAIN_MENU_BUTTON_NUMBER; ++i) {
		detailsPanels[i]->createWindow();
		detailsPanels[i]->setVisible(false);
	}


	for (int i = 0; i < MAIN_MENU_BUTTON_NUMBER; ++i) {
		Texture2D* texture2 = Game::get()->getCache()->GetResource<Texture2D
		>("textures/hud/icon/menu/menu_" + Urho3D::String(i) + ".png");

		MySprite* sprite2 = createSprite(texture2, style, "MainMenuSprite");
		Button* button = simpleButton(sprite2, style, "MainMenuButton");
		Text* text = button->CreateChild<Text>();
		String msg = l10n->Get("menu_" + String(i));
		text->SetText(msg);
		text->SetStyle("MainMenuText", style);

		HudElement* hudElement = new HudElement(button);
		hudElement->setId(i, ObjectType::ENTITY);
		buttons.push_back(hudElement);

		button->SetVar("HudElement", hudElement);
		window->AddChild(button);
		SubscribeToEvent(button, E_CLICK, URHO3D_HANDLER(MainMenuPanel, HandleButtonClick));
	}
}

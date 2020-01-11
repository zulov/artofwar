#include "InGameMenuPanel.h"
#include "GameState.h"
#include "hud/UiUtils.h"
#include "middle/FilePanel.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>


#define IN_GAME_MENU_BUTTON_NUMBER 5

InGameMenuPanel::InGameMenuPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style, "InGameMenuWindow",
                                                                               {GameState::RUNNING, GameState::PAUSE}) {
}

InGameMenuPanel::~InGameMenuPanel() {
	toggleButton->Remove();

	for (int i = 0; i < IN_GAME_MENU_BUTTON_NUMBER; ++i) {
		delete addionalPanels[i];
	}

	delete[]addionalPanels;
}

void InGameMenuPanel::setVisible(bool enable) {
	if (enable) {
		AbstractWindowPanel::setVisible(menuVisibility);
	} else {
		AbstractWindowPanel::setVisible(enable);
	}
	toggleButton->SetVisible(enable);
}

Urho3D::Button* InGameMenuPanel::getSaveButton() const {
	return addionalPanels[0]->getMainButton();
}

void InGameMenuPanel::toggle() {
	menuVisibility = !menuVisibility;
	setVisible(true);
}

void InGameMenuPanel::action(short id) {
	close();

	addionalPanels[id]->setVisible(true);
}

void InGameMenuPanel::close() const {
	for (int i = 0; i < IN_GAME_MENU_BUTTON_NUMBER; ++i) {
		addionalPanels[i]->setVisible(false);
	}
}

void InGameMenuPanel::createBody() {
	auto texture = Game::getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/igm/menu.png");
	toggleButton = createElement<Urho3D::Button>(Game::getUI()->GetRoot(), style, "InGameToggledButton");
	createSprite(toggleButton, texture, style, "InGameToggledSprite");

	SubscribeToEvent(toggleButton, Urho3D::E_CLICK, URHO3D_HANDLER(InGameMenuPanel, HandleToggle));

	for (int i = 0; i < IN_GAME_MENU_BUTTON_NUMBER; ++i) {
		auto texture2 = Game::getCache()->GetResource<Urho3D::Texture2D
		>("textures/hud/icon/igm/igm_" + Urho3D::String(i) + ".png");

		auto button = createElement<Urho3D::Button>(window, style, "InGameButton");
		createSprite(button, texture2, style, "InGameSprite");

		addChildText(button, "InGameText", Game::getLocalization()->Get("igm_" + Urho3D::String(i)), style);

		button->SetVar("Num", i);
		SubscribeToEvent(button, Urho3D::E_CLICK, URHO3D_HANDLER(InGameMenuPanel, HandleButtonClick));
	}

	auto l10n = Game::getLocalization();
	addionalPanels = new AbstractMiddlePanel*[IN_GAME_MENU_BUTTON_NUMBER];
	addionalPanels[0] = new FilePanel(style, l10n->Get("igm_0"));
	addionalPanels[1] = new FilePanel(style, l10n->Get("igm_1"));
	addionalPanels[2] = new AbstractMiddlePanel(style, l10n->Get("igm_2"));
	addionalPanels[3] = new AbstractMiddlePanel(style, l10n->Get("igm_3"));
	addionalPanels[4] = new AbstractMiddlePanel(style, l10n->Get("igm_4"));
	for (int i = 0; i < IN_GAME_MENU_BUTTON_NUMBER; ++i) {
		addionalPanels[i]->createWindow();
	}
}

void InGameMenuPanel::HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());

	action(element->GetVar("HudElement").GetInt());
}

void InGameMenuPanel::HandleToggle(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	toggle();
}

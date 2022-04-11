#include "InGameMenuPanel.h"

#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include "hud/UiUtils.h"
#include "middle/FilePanel.h"
#include "utils/DeleteUtils.h"

InGameMenuPanel::InGameMenuPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style)
	: EventPanel(root, _style, "InGameMenuWindow",
	             {GameState::RUNNING, GameState::PAUSE}) {}

InGameMenuPanel::~InGameMenuPanel() {
	toggleButton->Remove();
	clear_array(additionalPanels, IN_GAME_MENU_BUTTON_NUMBER);
}

void InGameMenuPanel::setVisible(bool enable) {
	if (enable) {
		SimplePanel::setVisible(menuVisibility);
	} else {
		SimplePanel::setVisible(enable);
	}
	toggleButton->SetVisible(enable);
}

Urho3D::Button* InGameMenuPanel::getSaveButton() const {
	return additionalPanels[0]->getMainButton();
}

void InGameMenuPanel::toggle() {
	menuVisibility = !menuVisibility;
	setVisible(true);
}

void InGameMenuPanel::action(short id) {
	close();

	additionalPanels[id]->setVisible(true);
}

void InGameMenuPanel::close() const {
	for (auto addionalPanel : additionalPanels) {
		addionalPanel->setVisible(false);
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
	additionalPanels[0] = new FilePanel(root, style, l10n->Get("igm_0"));
	additionalPanels[1] = new FilePanel(root, style, l10n->Get("igm_1"));
	additionalPanels[2] = new AbstractMiddlePanel(root, style, l10n->Get("igm_2"));
	additionalPanels[3] = new AbstractMiddlePanel(root, style, l10n->Get("igm_3"));
	additionalPanels[4] = new AbstractMiddlePanel(root, style, l10n->Get("igm_4"));
	for (auto addionalPanel : additionalPanels) {
		addionalPanel->createWindow();
	}
}

void InGameMenuPanel::HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());

	action(element->GetVar("HudElement").GetInt());
}

void InGameMenuPanel::HandleToggle(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	toggle();
}

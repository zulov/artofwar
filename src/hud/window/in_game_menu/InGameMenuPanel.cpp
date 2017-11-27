#include "InGameMenuPanel.h"
#include "Game.h"
#include <Urho3D/UI/UI.h>
#include "hud/MySprite.h"
#include "hud/ButtonUtils.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include "hud/window/middle/FilePanel.h"

#define IN_GAME_MENU_BUTTON_NUMBER 5

InGameMenuPanel::InGameMenuPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style) {
	styleName = "InGameMenuWindow";
}

InGameMenuPanel::~InGameMenuPanel() {
}

void InGameMenuPanel::setVisible(bool enable) {
	if (enable) {
		AbstractWindowPanel::setVisible(menuVisibility);
	} else {
		AbstractWindowPanel::setVisible(enable);
	}
	toggleButton->SetVisible(enable);
}

Urho3D::Button* InGameMenuPanel::getToggleButton() {
	return toggleButton;
}

std::vector<HudElement*>* InGameMenuPanel::getButtons() {
	return buttons;
}

std::vector<HudElement*>* InGameMenuPanel::getClosedButtons() {
	return new std::vector<HudElement*>();
}

void InGameMenuPanel::toggle() {
	menuVisibility = !menuVisibility;
	setVisible(true);
}

void InGameMenuPanel::action(short id) {
	addionalPanels[id]->setVisible(true);
}

void InGameMenuPanel::close() {

	for (int i = 0; i < IN_GAME_MENU_BUTTON_NUMBER; ++i) {
		addionalPanels[i]->setVisible(false);
	}
}

void InGameMenuPanel::createBody() {
	Urho3D::Texture2D* texture = Game::get()->getCache()->GetResource<Urho3D::Texture2D>("textures/hud/icon/igm/menu.png");

	MySprite* sprite = createSprite(texture, style, "InGameToggledSprite");
	toggleButton = simpleButton(sprite, style, "InGameToggledButton");
	Game::get()->getUI()->GetRoot()->AddChild(toggleButton);

	buttons = new std::vector<HudElement*>();
	for (int i = 0; i < IN_GAME_MENU_BUTTON_NUMBER; ++i) {
		Texture2D* texture2 = Game::get()->getCache()->GetResource<Texture2D
		>("textures/hud/icon/igm/igm_" + String(i) + ".png");

		MySprite* sprite2 = createSprite(texture2, style, "InGameSprite");
		Button* button = simpleButton(sprite2, style, "InGameButton");
		Text* text = button->CreateChild<Text>();
		String msg = Game::get()->getLocalization()->Get("igm_" + String(i));
		text->SetText(msg);
		text->SetStyle("InGameText", style);

		HudElement* hudElement = new HudElement(button);
		hudElement->setId(i, ObjectType::ENTITY);
		buttons->push_back(hudElement);

		button->SetVar("HudElement", hudElement);
		window->AddChild(button);
	}

	addionalPanels = new AbstractMiddlePanel*[IN_GAME_MENU_BUTTON_NUMBER];
	addionalPanels[0] = new FilePanel(style);
	addionalPanels[1] = new FilePanel(style);
	addionalPanels[2] = new AbstractMiddlePanel(style);
	addionalPanels[3] = new AbstractMiddlePanel(style);
	addionalPanels[4] = new AbstractMiddlePanel(style);
	for (int i = 0; i < IN_GAME_MENU_BUTTON_NUMBER; ++i) {
		addionalPanels[i]->createWindow();
	}
}

#include "InGameMenuPanel.h"
#include "Game.h"
#include <Urho3D/UI/UI.h>
#include "hud/MySprite.h"
#include "hud/ButtonUtils.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>

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
		additionalWindow->SetVisible(enable);
	}
	toggleButton->SetVisible(enable);
}

Urho3D::Button* InGameMenuPanel::getToggleButton() {
	return toggleButton;
}

Urho3D::Button* InGameMenuPanel::getCloseButton() {
	return buttonClose;
}

std::vector<HudElement*>* InGameMenuPanel::getButtons() {
	return buttons;
}

void InGameMenuPanel::toggle() {
	menuVisibility = !menuVisibility;
	setVisible(true);
}

void InGameMenuPanel::action(short id) {
	additionalWindow->SetVisible(true);
}

void InGameMenuPanel::close() {
	additionalWindow->SetVisible(false);
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

	additionalWindow = new Urho3D::Window(Game::get()->getContext());
	additionalWindow->SetStyle("AdditionalInGameWindow", style);
	Game::get()->getUI()->GetRoot()->AddChild(additionalWindow);

	buttonClose = additionalWindow->CreateChild<Button>();
	buttonClose->SetStyle("MyCloseButton");
}

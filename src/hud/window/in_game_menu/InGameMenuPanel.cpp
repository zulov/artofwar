#include "InGameMenuPanel.h"
#include "Game.h"
#include <Urho3D/UI/UI.h>
#include "hud/MySprite.h"
#include "hud/ButtonUtils.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Text.h>
#include "middle/FilePanel.h"
#include <Urho3D/UI/UIEvents.h>
#include "utils.h"

#define IN_GAME_MENU_BUTTON_NUMBER 5

InGameMenuPanel::InGameMenuPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style) {
	styleName = "InGameMenuWindow";
}

InGameMenuPanel::~InGameMenuPanel() {
	toggleButton->Remove();
	clear_vector(buttons);

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

Urho3D::Button* InGameMenuPanel::getSaveButton() {
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
	SubscribeToEvent(toggleButton, E_CLICK, URHO3D_HANDLER(InGameMenuPanel, HandleToggle));

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
		buttons.push_back(hudElement);

		button->SetVar("HudElement", hudElement);
		window->AddChild(button);
		SubscribeToEvent(button, E_CLICK, URHO3D_HANDLER(InGameMenuPanel, HandleButtonClick));
	}

	Localization* l10n = Game::get()->getLocalization();
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

void InGameMenuPanel::HandleButtonClick(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	HudElement* hudElement = static_cast<HudElement *>(element->GetVar("HudElement").GetVoidPtr());

	action(hudElement->getId());
}

void InGameMenuPanel::HandleToggle(StringHash eventType, VariantMap& eventData) {
	toggle();
}

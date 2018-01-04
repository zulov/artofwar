#include "MainMenuPanel.h"
#include "hud/MySprite.h"
#include "hud/ButtonUtils.h"
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>


MainMenuPanel::MainMenuPanel(Urho3D::XMLFile* _style): AbstractWindowPanel(_style) {
	styleName = "MainMenuWindow";
}


MainMenuPanel::~MainMenuPanel() {
	delete detailsPanel;
}

void MainMenuPanel::setVisible(bool enable) {
	AbstractWindowPanel::setVisible(enable);
	detailsPanel->setVisible(enable);
}

void MainMenuPanel::createBody() {
	detailsPanel = new MainMenuDetailsPanel(style);
	detailsPanel->createWindow();

	for (int i = 0; i < MAIN_MENU_BUTTON_NUMBER; ++i) {
		Texture2D* texture2 = Game::get()->getCache()->GetResource<Texture2D
		>("textures/hud/icon/menu/menu_" + Urho3D::String(i) + ".png");

		MySprite* sprite2 = createSprite(texture2, style, "MainMenuSprite");
		Button* button = simpleButton(sprite2, style, "MainMenuButton");
		Text* text = button->CreateChild<Text>();
		String msg = Game::get()->getLocalization()->Get("menu_" + String(i));
		text->SetText(msg);
		text->SetStyle("MainMenuText", style);

		HudElement* hudElement = new HudElement(button);
		hudElement->setId(i, ObjectType::ENTITY);
		buttons.push_back(hudElement);

		button->SetVar("HudElement", hudElement);
		window->AddChild(button);
		//SubscribeToEvent(button, E_CLICK, URHO3D_HANDLER(InGameMenuPanel, HandleButtonClick));
	}
}

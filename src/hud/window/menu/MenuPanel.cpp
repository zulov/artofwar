#include "MenuPanel.h"
#include "Game.h"
#include "GameState.h"
#include "database/DatabaseCache.h"
#include <Urho3D/Resource/ResourceCache.h>
#include "hud/MySprite.h"
#include "hud/UiUtils.h"
#include "info/LeftMenuInfoPanel.h"
#include <Urho3D/UI/CheckBox.h>
#include "hud/window/in_game_menu/middle/AbstractMiddlePanel.h"


MenuPanel::MenuPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "LeftMenuWindow";
	visibleAt.insert(GameState::RUNNING);
	visibleAt.insert(GameState::PAUSE);
}


MenuPanel::~MenuPanel() = default;

void MenuPanel::removeInfo() {
	infoPanel->setVisible(false);
}

void MenuPanel::setInfo(HudElement* hudElement) {
	infoPanel->setInfo(hudElement);
}

void MenuPanel::updateSelected(SelectedInfo* selectedInfo) {
	infoPanel->updateSelected(selectedInfo);
}

void MenuPanel::setVisible(bool enable) {
	AbstractWindowPanel::setVisible(enable);
	if (!enable) {
		infoPanel->setVisible(false);
	}
}

void MenuPanel::createBody() {
	infoPanel = new LeftMenuInfoPanel(style);
	infoPanel->createWindow();

	mock = window->CreateChild<UIElement>();
	mock->SetStyle("LeftMenuMock", style);

	for (int i = 0; i < LEFT_MENU_ROWS_NUMBER; ++i) {
		rows[i] = mock->CreateChild<UIElement>();
		rows[i]->SetStyle("LeftMenuListRow", style);
	}
	for (int i = 0; i < LEFT_MENU_CHECKS_NUMBER; ++i) {
		Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D
		>("textures/hud/icon/lm/lm" + String(i) + ".png");

		MySprite* sprite = createSprite(texture, style, "LeftMenuSmallSprite");
		checks[i] = rows[LEFT_MENU_ROWS_NUMBER - 1]->CreateChild<CheckBox>();
		checks[i]->SetStyle("LeftMenuCheckBox", style);

		checks[i]->AddChild(sprite);
	}
	Texture2D* texture = Game::get()->getCache()->GetResource<Texture2D
	>("textures/hud/icon/lm/lm3.png");
	MySprite* sprite = createSprite(texture, style, "LeftMenuSmallSprite");
	nextButton = rows[LEFT_MENU_ROWS_NUMBER - 1]->CreateChild<Button>();
	nextButton->SetStyle("LeftMenuIcon", style);
	nextButton->AddChild(sprite);

	int k = 0;
	for (int i = 0; i < LEFT_MENU_ROWS_NUMBER - 1; ++i) {
		for (int j = 0; j < LEFT_MENU_BUTTON_PER_ROW; ++j) {

			buttons[k] = rows[i]->CreateChild<Button>();
			buttons[k]->SetStyle("LeftMenuBigIcon", style);
			k++;
		}
	}

}

#include "MainMenuNewGamePanel.h"
#include "Game.h"
#include <Urho3D/UI/DropDownList.h>
#include "database/DatabaseCache.h"
#include <Urho3D/UI/LineEdit.h>


MainMenuNewGamePanel::
MainMenuNewGamePanel(Urho3D::XMLFile* _style, Urho3D::String _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuNewGameMock";
}

void MainMenuNewGamePanel::createBody() {
	MainMenuDetailsPanel::createBody();
	for (int i = 0; i < NEW_GAME_ROWS_NUMBER; ++i) {
		rows[i] = body->CreateChild<Urho3D::BorderImage>();
		rows[i]->SetStyle("MainMenuNewGameRow");
	}
	Urho3D::Localization* l10n = Game::get()->getLocalization();


	populateLabels(l10n, 0, "player");
	populateLabels(l10n, 1, "enemy");
	populateLabels(l10n, 2, "map_name");

	myLine.populateTeams(l10n, rows[0], style);
	enemyLine.populateTeams(l10n, rows[1], style);

	proceed = body->CreateChild<Urho3D::Button>();
	proceed->SetStyle("MainMenuNewGameButton");
	Urho3D::Text* text = proceed->CreateChild<Urho3D::Text>();
	text->SetStyle("MainMenuNewGameButtonText", style);
	text->SetText(l10n->Get("start"));
}

void MainMenuNewGamePanel::populateLabels(Urho3D::Localization* l10n, int index, Urho3D::String name) {
	Urho3D::Text* text = rows[index]->CreateChild<Urho3D::Text>();
	text->SetStyle("MainMenuNewGameButtonLabel", style);
	text->SetText(l10n->Get(name));
}


MainMenuNewGamePanel::~MainMenuNewGamePanel() {
}

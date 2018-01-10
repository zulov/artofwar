#include "MainMenuNewGamePanel.h"
#include "Game.h"
#include <Urho3D/UI/DropDownList.h>
#include "database/DatabaseCache.h"


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

	populateTeams(l10n, 0, "player");
	populateTeams(l10n, 1, "enemy");

	proceed = body->CreateChild<Urho3D::Button>();
	proceed->SetStyle("MainMenuNewGameButton");
	Urho3D::Text* text = proceed->CreateChild<Urho3D::Text>();
	text->SetStyle("MyText", style);
	text->SetText(l10n->Get("start"));
}


void MainMenuNewGamePanel::populateTeams(Urho3D::Localization* l10n, int index, Urho3D::String name) {
	Urho3D::Text* text = rows[index]->CreateChild<Urho3D::Text>();
	text->SetStyle("MyText", style);
	text->SetText(l10n->Get(name));

	Urho3D::DropDownList* nation = rows[index]->CreateChild<Urho3D::DropDownList>();
	nation->SetStyle("MainMenuNewGameDropDownList", style);
	int size = Game::get()->getDatabaseCache()->getNationSize();
	for (int i = 0; i < size; ++i) {
		Urho3D::Text* item = new Urho3D::Text(Game::get()->getContext());
		item->SetStyle("MyText", style);
		item->SetText(
		              l10n->Get(
		                        "nation_" + Game::get()->getDatabaseCache()->getNation(i)->name
		                       ));
		nation->AddItem(item);
	}
	Urho3D::DropDownList* color = rows[index]->CreateChild<Urho3D::DropDownList>();
	color->SetStyle("MainMenuNewGameDropDownList", style);
	int sizeColor = Game::get()->getDatabaseCache()->getPlayerColorsSize();
	for (int i = 0; i < sizeColor; ++i) {
		Urho3D::Text* item = new Urho3D::Text(Game::get()->getContext());
		item->SetStyle("MyText", style);
		item->SetText(
		              l10n->Get(
		                        "color_" + Game::get()->getDatabaseCache()->getPlayerColor(i)->name
		                       ));

		color->AddItem(item);
	}
}


MainMenuNewGamePanel::~MainMenuNewGamePanel() {
}

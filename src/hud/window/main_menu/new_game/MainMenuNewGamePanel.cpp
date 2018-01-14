#include "MainMenuNewGamePanel.h"
#include "Game.h"
#include "database/DatabaseCache.h"
#include <ctime>
#include <Urho3D/UI/UIEvents.h>
#include "hud/HudElement.h"
#include "NewGameForm.h"


MainMenuNewGamePanel::
MainMenuNewGamePanel(Urho3D::XMLFile* _style, Urho3D::String _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuNewGameMock";
	srand(time(NULL));
}

void MainMenuNewGamePanel::createBody() {
	MainMenuDetailsPanel::createBody();
	for (int i = 0; i < NEW_GAME_ROWS_NUMBER; ++i) {
		rows[i] = body->CreateChild<Urho3D::BorderImage>();
		rows[i]->SetStyle("MainMenuNewGameRow");
	}
	Urho3D::Localization* l10n = Game::get()->getLocalization();
	myLine.init(style, l10n);
	enemyLine.init(style, l10n);

	populateLabels(l10n, 0, "player");
	populateLabels(l10n, 1, "enemy");
	populateLabels(l10n, 2, "map_name");
	populateLabels(l10n, 3, "difficulty");
	populateLabels(l10n, 4, "game_speed");

	myLine.populateTeams(rows[0]);
	enemyLine.populateTeams(rows[1]);

	map = createDropDownList(rows[2], "MainMenuNewGameDropDownList", style);
	int mapsSize = Game::get()->getDatabaseCache()->getMapSize();
	for (int i = 0; i < mapsSize; ++i) {
		addTextItem(map, Game::get()->getDatabaseCache()->getMap(i)->name, style);
	}

	difficulty = createDropDownList(rows[3], "MainMenuNewGameDropDownList", style);
	addTextItem(difficulty, l10n->Get("easy"), style);
	addTextItem(difficulty, l10n->Get("normal"), style);
	addTextItem(difficulty, l10n->Get("hard"), style);

	gameSpeed = createDropDownList(rows[4], "MainMenuNewGameDropDownList", style);
	addTextItem(gameSpeed, l10n->Get("slow"), style);
	addTextItem(gameSpeed, l10n->Get("normal"), style);
	addTextItem(gameSpeed, l10n->Get("fast"), style);

	proceed = body->CreateChild<Urho3D::Button>();
	proceed->SetStyle("MainMenuNewGameButton");
	data = new NewGameForm();
	proceed->SetVar("NewGameForm", data);

	SubscribeToEvent(proceed, E_CLICK, URHO3D_HANDLER(MainMenuNewGamePanel, HandleNewGame));

	addChildText(proceed, "MainMenuNewGameButtonText", l10n->Get("start"), style);
}

Button* MainMenuNewGamePanel::getProceed() {
	return proceed;
}

void MainMenuNewGamePanel::HandleNewGame(StringHash eventType, VariantMap& eventData) {
	data->map=map->GetSelection();
	data->difficulty=difficulty->GetSelection();
	data->gameSpeed=gameSpeed->GetSelection();
	data->playerOne = myLine.getNewGamePlayer();
	data->enemy = enemyLine.getNewGamePlayer();

}

void MainMenuNewGamePanel::populateLabels(Urho3D::Localization* l10n, int index, Urho3D::String name) {
	Urho3D::Text* text = rows[index]->CreateChild<Urho3D::Text>();
	text->SetStyle("MainMenuNewGameButtonLabel", style);
	text->SetText(l10n->Get(name));
}


MainMenuNewGamePanel::~MainMenuNewGamePanel() {
	delete data;
}

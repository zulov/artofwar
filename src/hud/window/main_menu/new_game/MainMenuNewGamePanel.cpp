#include "MainMenuNewGamePanel.h"
#include "Game.h"
#include "NewGameForm.h"
#include "database/DatabaseCache.h"
#include "hud/HudData.h"
#include <Urho3D/UI/UIEvents.h>
#include <ctime>


MainMenuNewGamePanel::
MainMenuNewGamePanel(Urho3D::XMLFile* _style, const Urho3D::String& _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuNewGameMock";
	srand(time(NULL));
}

void MainMenuNewGamePanel::createBody() {
	MainMenuDetailsPanel::createBody();

	for (auto& row : rows) {
		row = createElement<Urho3D::BorderImage>(body, style,"MainMenuNewGameRow");
	}
	auto l10n = Game::getLocalization();
	myLine.init(style, l10n, 0);
	enemyLine.init(style, l10n, 1);

	populateLabels(l10n, 0, "player");
	populateLabels(l10n, 1, "enemy");
	populateLabels(l10n, 2, "map_name");
	populateLabels(l10n, 3, "difficulty");
	populateLabels(l10n, 4, "game_speed");

	myLine.populateTeams(rows[0]);
	enemyLine.populateTeams(rows[1]);
	myLine.setCheck(true);

	map = createElement<Urho3D::DropDownList>(rows[2], style, "MainMenuNewGameDropDownList");
	int mapsSize = Game::getDatabaseCache()->getMapSize();
	for (int i = 0; i < mapsSize; ++i) {
		addTextItem(map, Game::getDatabaseCache()->getMap(i)->name, style);
	}

	difficulty = createElement<Urho3D::DropDownList>(rows[3], style, "MainMenuNewGameDropDownList");
	addChildTexts(difficulty, {l10n->Get("easy"), l10n->Get("normal"), l10n->Get("hard")}, style);

	gameSpeed = createElement<Urho3D::DropDownList>(rows[4], style, "MainMenuNewGameDropDownList");
	addChildTexts(gameSpeed, {l10n->Get("slow"), l10n->Get("normal"), l10n->Get("fast")}, style);

	proceed = createElement<Urho3D::Button>(body, style,"MainMenuNewGameButton");
	data = new NewGameForm();
	proceed->SetVar("NewGameForm", data);

	SubscribeToEvent(proceed, Urho3D::E_CLICK, URHO3D_HANDLER(MainMenuNewGamePanel, HandleNewGame));
	SubscribeToEvent(myLine.getCheckBox(), Urho3D::E_CLICK, URHO3D_HANDLER(MainMenuNewGamePanel, HandleCheck));
	SubscribeToEvent(enemyLine.getCheckBox(), Urho3D::E_CLICK, URHO3D_HANDLER(MainMenuNewGamePanel, HandleCheck));

	addChildText(proceed, "MainMenuNewGameButtonText", l10n->Get("start"), style);
}

Urho3D::Button* MainMenuNewGamePanel::getProceed() const {
	return proceed;
}

void MainMenuNewGamePanel::HandleCheck(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	myLine.setCheck(false);
	enemyLine.setCheck(false);

	auto element = static_cast<Urho3D::CheckBox*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	element->SetChecked(true);
}

void MainMenuNewGamePanel::HandleNewGame(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	data->map = map->GetSelection();
	data->difficulty = difficulty->GetSelection();
	data->gameSpeed = gameSpeed->GetSelection();
	data->players.push_back(myLine.getNewGamePlayer());
	data->players.push_back(enemyLine.getNewGamePlayer());
}

void MainMenuNewGamePanel::populateLabels(Urho3D::Localization* l10n, int index, const Urho3D::String& name) {
	addChildText(rows[index], "MainMenuNewGameButtonLabel", l10n->Get(name), style);
}


MainMenuNewGamePanel::~MainMenuNewGamePanel() {
	delete data;
}

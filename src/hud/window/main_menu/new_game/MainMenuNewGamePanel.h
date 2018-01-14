#pragma once
#include "NewGameTeamLine.h"
#include "hud/window/main_menu/MainMenuDetailsPanel.h"
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/Button.h>
#include "NewGameForm.h"


#define NEW_GAME_ROWS_NUMBER 5

class MainMenuNewGamePanel : public MainMenuDetailsPanel
{
public:
	MainMenuNewGamePanel(Urho3D::XMLFile* _style, Urho3D::String _title);
	Button* getProceed();
	void HandleNewGame(StringHash eventType, VariantMap& eventData);
	~MainMenuNewGamePanel();
private:
	void populateLabels(Urho3D::Localization* localization, int i, Urho3D::String name);

	void createBody() override;

	Urho3D::BorderImage* rows[NEW_GAME_ROWS_NUMBER];
	NewGameTeamLine myLine;
	NewGameTeamLine enemyLine;
	Urho3D::DropDownList* map;
	Urho3D::DropDownList* difficulty;
	Urho3D::DropDownList* gameSpeed;
	Urho3D::Button* proceed;
	NewGameForm* data;
};

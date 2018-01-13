#pragma once
#include "hud/window/main_menu/MainMenuDetailsPanel.h"
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/Button.h>
#include "NewGameTeamLine.h"

#define NEW_GAME_ROWS_NUMBER 5

class MainMenuNewGamePanel:public MainMenuDetailsPanel
{
public:
	MainMenuNewGamePanel(Urho3D::XMLFile* _style, Urho3D::String _title);
	~MainMenuNewGamePanel();
private:
	void populateLabels(Urho3D::Localization* localization, int i, Urho3D::String name);
	void createBody() override;

	Urho3D::BorderImage * rows[NEW_GAME_ROWS_NUMBER];
	NewGameTeamLine myLine;
	NewGameTeamLine enemyLine;
	Urho3D::Button * proceed;
};




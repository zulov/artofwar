#pragma once
#include "hud/window/main_menu/MainMenuDetailsPanel.h"

#define NEW_GAME_ROWS_NUMBER 5

class MainMenuNewGamePanel:public MainMenuDetailsPanel
{
public:
	MainMenuNewGamePanel(Urho3D::XMLFile* _style, Urho3D::String _title);
	~MainMenuNewGamePanel();
private:
	void createBody() override;

	Urho3D::BorderImage * rows[NEW_GAME_ROWS_NUMBER];
};




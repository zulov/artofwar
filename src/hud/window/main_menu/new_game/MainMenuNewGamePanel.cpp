#include "MainMenuNewGamePanel.h"


MainMenuNewGamePanel::
MainMenuNewGamePanel(Urho3D::XMLFile* _style, Urho3D::String _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuNewGameMock";
}

void MainMenuNewGamePanel::createBody() {
	MainMenuDetailsPanel::createBody();
	for (int i = 0; i < NEW_GAME_ROWS_NUMBER; ++i) {
		rows[i]= body->CreateChild<Urho3D::BorderImage>();
		rows[i]->SetStyle("MainMenuNewGameRow");
	}
	
}


MainMenuNewGamePanel::~MainMenuNewGamePanel() {
}

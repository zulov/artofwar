#include "MainMenuPanel.h"


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
}

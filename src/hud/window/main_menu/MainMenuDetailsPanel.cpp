#include "MainMenuDetailsPanel.h"


MainMenuDetailsPanel::MainMenuDetailsPanel( Urho3D::String _title): AbstractWindowPanel() {
	styleName = "MainMenuDetailsWindow";
	bodyStyle = "MainMenuMock";
	msg = _title;
}


MainMenuDetailsPanel::~MainMenuDetailsPanel() = default;

void MainMenuDetailsPanel::createBody() {
	title = window->CreateChild<Urho3D::Text>();
	title->SetStyle("MainMenuTitle");
	title->SetText(msg);

	body = window->CreateChild<Urho3D::UIElement>();
	body->SetStyle(bodyStyle);
	window->SetPriority(2);
}

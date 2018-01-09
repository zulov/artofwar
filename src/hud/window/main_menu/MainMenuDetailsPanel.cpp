#include "MainMenuDetailsPanel.h"


MainMenuDetailsPanel::MainMenuDetailsPanel(Urho3D::XMLFile* _style, Urho3D::String _title): AbstractWindowPanel(_style) {
	styleName = "MainMenuDetailsWindow";
	bodyStyle = "MainMenuMock";
	msg = _title;
}


MainMenuDetailsPanel::~MainMenuDetailsPanel() = default;

void MainMenuDetailsPanel::createBody() {
	title = window->CreateChild<Urho3D::Text>();
	title->SetStyle("MainMenuTitle", style);
	title->SetText(msg);

	body = window->CreateChild<Urho3D::UIElement>();
	body->SetStyle(bodyStyle, style);
}

#include "MainMenuDetailsPanel.h"
#include "Game.h"
#include "hud/UiUtils.h"


MainMenuDetailsPanel::
MainMenuDetailsPanel(Urho3D::XMLFile* _style, const Urho3D::String& _title):
	AbstractWindowPanel(_style, "MainMenuDetailsWindow", {}) {
	bodyStyle = "MainMenuMock";
	msg = _title;
}


MainMenuDetailsPanel::~MainMenuDetailsPanel() = default;

void MainMenuDetailsPanel::createBody() {
	addChildText(window, "MainMenuTitle", msg, style);

	body = window->CreateChild<Urho3D::UIElement>();
	body->SetStyle(bodyStyle, style);
	window->SetPriority(2);
}

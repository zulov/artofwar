#include "MainMenuDetailsPanel.h"
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

	body = createElement<Urho3D::UIElement>(window, style, "bodyStyle");
	window->SetPriority(2);
}

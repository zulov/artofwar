#include "MainMenuDetailsPanel.h"
#include <Urho3D/UI/Window.h>
#include "hud/UiUtils.h"


MainMenuDetailsPanel::MainMenuDetailsPanel(Urho3D::XMLFile* _style, const Urho3D::String& _title)
	: EventPanel(_style, "MainMenuDetailsWindow", {}) {
	bodyStyle = "MainMenuMock";
	msg = _title;
}

void MainMenuDetailsPanel::createBody() {
	addChildText(window, "MainMenuTitle", msg, style);

	body = createElement<Urho3D::UIElement>(window, style, "bodyStyle");
	window->SetPriority(2);
}

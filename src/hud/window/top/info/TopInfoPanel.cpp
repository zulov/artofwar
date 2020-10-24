#include "TopInfoPanel.h"
#include <Urho3D/UI/Window.h>
#include "hud/UiUtils.h"


TopInfoPanel::TopInfoPanel(Urho3D::XMLFile* _style) : SimplePanel(_style, "TopInfoPanel", {}) {
}

void TopInfoPanel::hoverOn() {
	setVisible(true);
}

void TopInfoPanel::hoverOff() {
	setVisible(false);
}

void TopInfoPanel::createBody() {
	text = addChildText(window, "MyText", style);
}

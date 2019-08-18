#include "TopInfoPanel.h"
#include "hud/UiUtils.h"


TopInfoPanel::TopInfoPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "TopInfoPanel", {}) {
}


TopInfoPanel::~TopInfoPanel() = default;

void TopInfoPanel::hoverOff() {
	setVisible(false);
}

void TopInfoPanel::createBody() {
	text = addChildText(window, "MyText", style);
}

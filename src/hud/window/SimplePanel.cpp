#include "SimplePanel.h"

#include <Urho3D/UI/Window.h>
#include "hud/UiUtils.h"
#include "utils/OtherUtils.h"


SimplePanel::SimplePanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style, Urho3D::String styleName,
                         std::initializer_list<GameState> active):
	style(_style), styleName(std::move(styleName)), root(root) {
	std::fill_n(visibleAt, magic_enum::enum_count<GameState>(), false);
	for (auto a : active) {
		visibleAt[cast(a)] = true;
	}
}

SimplePanel::~SimplePanel() {
	window->Remove();
}

void SimplePanel::createWindow() {
	window = createElement<Urho3D::Window>(root, style, styleName);
	createBody();
}

void SimplePanel::updateStateVisibility(GameState state) {
	setVisible(visibleAt[cast(state)]);
}

void SimplePanel::setVisible(bool enable) {
	window->SetVisible(enable);
}

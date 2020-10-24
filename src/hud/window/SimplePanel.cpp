#include "SimplePanel.h"

#include <magic_enum.hpp>
#include <utility>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>
#include "hud/UiUtils.h"


SimplePanel::SimplePanel(Urho3D::XMLFile* _style, Urho3D::String styleName,
                         std::initializer_list<GameState> active):
	style(_style), styleName(std::move(styleName)) {
	std::fill_n(visibleAt, magic_enum::enum_count<GameState>(), false);
	for (auto a : active) {
		visibleAt[static_cast<char>(a)] = true;
	}
}


SimplePanel::~SimplePanel() {
	window->Remove();
}

void SimplePanel::createWindow() {
	window = createElement<Urho3D::Window>(Game::getUI()->GetRoot(), style, styleName);
	createBody();
}

void SimplePanel::updateStateVisibility(GameState state) {
	setVisible(visibleAt[static_cast<char>(state)]);
}

void SimplePanel::setVisible(bool enable) {
	window->SetVisible(enable);
}

#include "AbstractWindowPanel.h"

#include <magic_enum.hpp>
#include <utility>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Window.h>
#include "Game.h"
#include "hud/UiUtils.h"


AbstractWindowPanel::AbstractWindowPanel(Urho3D::XMLFile* _style, Urho3D::String styleName,
                                         std::initializer_list<GameState> active): Object(Game::getContext()),
	style(_style), styleName(
		std::move(styleName)) {
	std::fill_n(visibleAt, magic_enum::enum_count<GameState>(), false);
	for (auto a : active) {
		visibleAt[static_cast<char>(a)] = true;
	}
}


AbstractWindowPanel::~AbstractWindowPanel() {
	window->Remove();
}

void AbstractWindowPanel::createWindow() {
	window = createElement<Urho3D::Window>(Game::getUI()->GetRoot(), style, getStyleName());
	createBody();
}

void AbstractWindowPanel::updateStateVisibilty(GameState state) {
	setVisible(visibleAt[static_cast<char>(state)]);
}

void AbstractWindowPanel::setVisible(bool enable) {
	window->SetVisible(enable);
}

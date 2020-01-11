#include "AbstractWindowPanel.h"
#include "Game.h"
#include "hud/UiUtils.h"
#include <Urho3D/UI/UI.h>

#include <utility>


AbstractWindowPanel::AbstractWindowPanel(Urho3D::XMLFile* _style, Urho3D::String styleName,
                                         std::initializer_list<GameState> active): Object(Game::getContext()),
                                                                                   style(_style), styleName(
	                                                                                   std::move(styleName)) {
	std::fill_n(visibleAt, GAME_STATE_SIZE, false);
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

#include "AbstractWindowPanel.h"
#include "Game.h"
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>


AbstractWindowPanel::AbstractWindowPanel(Urho3D::XMLFile* _style, Urho3D::String styleName): Object(Game::getContext()),
	style(_style), styleName(styleName) {
	std::fill_n(visibleAt, GAME_STATE_SIZE, false);
}


AbstractWindowPanel::~AbstractWindowPanel() {
	window->Remove();
}

Urho3D::Window* AbstractWindowPanel::createWindow() {
	window = Game::getUI()->GetRoot()->CreateChild<Urho3D::Window>();
	window->SetStyle(getStyleName(), style);

	createBody();

	return window;
}

void AbstractWindowPanel::updateStateVisibilty(GameState state) {
	setVisible(visibleAt[static_cast<char>(state)]);
}

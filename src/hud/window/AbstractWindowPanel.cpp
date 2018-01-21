#include "AbstractWindowPanel.h"
#include "Game.h"
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>


AbstractWindowPanel::AbstractWindowPanel(): Object(Game::get()->getContext()) {
}


AbstractWindowPanel::~AbstractWindowPanel() {
	window->Remove();
}

Urho3D::String& AbstractWindowPanel::getStyleName() {
	return styleName;
}

Urho3D::Window* AbstractWindowPanel::createWindow() {
	window = Game::get()->getUI()->GetRoot()->CreateChild<Urho3D::Window>();
	window->SetStyle(getStyleName());

	createBody();

	return window;
}

void AbstractWindowPanel::setVisible(bool enable) {
	window->SetVisible(enable);
}

void AbstractWindowPanel::updateStateVisibilty(GameState state) {
	setVisible(visibleAt.find(state) != visibleAt.end());
}

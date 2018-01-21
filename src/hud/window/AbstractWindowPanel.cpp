#include "AbstractWindowPanel.h"
#include "Game.h"
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>


AbstractWindowPanel::AbstractWindowPanel(Urho3D::XMLFile* _style): Object(Game::get()->getContext()) {
	style = _style;
}


AbstractWindowPanel::~AbstractWindowPanel() {
	window->Remove();
}

Urho3D::String& AbstractWindowPanel::getStyleName() {
	return styleName;
}

Urho3D::Window* AbstractWindowPanel::createWindow() {
	window = Game::get()->getUI()->GetRoot()->CreateChild<Urho3D::Window>();
	window->SetStyle(getStyleName(), style);

	createBody();

	return window;
}

void AbstractWindowPanel::setVisible(bool enable) {
	window->SetVisible(enable);
}

void AbstractWindowPanel::updateStateVisibilty(GameState state) {
	setVisible(visibleAt.find(state) != visibleAt.end());
}

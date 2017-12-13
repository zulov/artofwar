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
	window = new Urho3D::Window(Game::get()->getContext());
	window->SetStyle(getStyleName(), style);
	Game::get()->getUI()->GetRoot()->AddChild(window);

	createBody();
	
	return window;
}

void AbstractWindowPanel::setVisible(bool enable) {
	window->SetVisible(enable);
}


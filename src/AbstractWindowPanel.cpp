#include "AbstractWindowPanel.h"
#include "Game.h"
#include <Urho3D/UI/Text.h>


AbstractWindowPanel::AbstractWindowPanel(Urho3D::XMLFile* _style) {
	style = _style;
}


AbstractWindowPanel::~AbstractWindowPanel() {
}

Urho3D::Window* AbstractWindowPanel::createWindow(const Urho3D::String& styleName) {
	window = new Urho3D::Window(Game::get()->getContext());
	//windows->push_back(window);
	window->SetStyle(styleName, style);
	Game::get()->getUI()->GetRoot()->AddChild(window);

	createBody();
	

	return window;
}

void AbstractWindowPanel::setVisible(bool enable) {
	window->SetVisible(enable);
}


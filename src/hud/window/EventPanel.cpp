#include "EventPanel.h"

#include "Game.h"

EventPanel::EventPanel(Urho3D::XMLFile* _style, Urho3D::String styleName,
                       std::initializer_list<GameState> active)
	: SimplePanel(_style, styleName, active),
	  Object(Game::getContext()) { }

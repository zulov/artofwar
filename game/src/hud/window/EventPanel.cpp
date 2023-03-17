#include "EventPanel.h"

#include <utility>

#include "Game.h"

EventPanel::EventPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style, Urho3D::String styleName,
                       std::initializer_list<GameState> active)
	: SimplePanel(root, _style, std::move(styleName), active),
	  Object(Game::getContext()) { }

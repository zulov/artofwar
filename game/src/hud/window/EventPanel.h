#pragma once
#include "SimplePanel.h"
#include "Urho3D/Core/Object.h"

class EventPanel : public SimplePanel, public Urho3D::Object {
URHO3D_OBJECT(EventPanel, Object)
public:
	EventPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style, Urho3D::String styleName,
	           std::initializer_list<GameState> active);
	virtual ~EventPanel() = default;

};

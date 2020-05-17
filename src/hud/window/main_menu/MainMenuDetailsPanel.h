#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>

class MainMenuDetailsPanel : public AbstractWindowPanel {
public:
	MainMenuDetailsPanel(Urho3D::XMLFile* _style, const Urho3D::String& _title);
	~MainMenuDetailsPanel() = default;
protected:
	Urho3D::UIElement* body;

	void createBody() override;
private:
	Urho3D::String msg;
};

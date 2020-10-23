#pragma once
#include "hud/window/AbstractWindowPanel.h"

namespace Urho3D {
	class UIElement;
}

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

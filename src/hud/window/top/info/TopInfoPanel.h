#pragma once
#include "hud/window/SimplePanel.h"

namespace Urho3D {
	class Text;
}

class TopInfoPanel : public SimplePanel {
public:
	explicit TopInfoPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style);
	~TopInfoPanel() = default;

	void hoverOn();
	void hoverOff();

private:
	void createBody() override;

	bool hoverIsOn = false;
	Urho3D::Text* text;
};

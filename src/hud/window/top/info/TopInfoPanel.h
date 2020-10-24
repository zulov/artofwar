#pragma once
#include <Urho3D/UI/Text.h>
#include "hud/window/SimplePanel.h"

class TopInfoPanel : public SimplePanel {
public:
	explicit TopInfoPanel(Urho3D::XMLFile* _style);
	~TopInfoPanel() = default;

	void hoverOn();
	void hoverOff();

private:
	//Urho3D::String createMessage(HudData* hudData);
	void createBody() override;

	bool hoverIsOn = false;
	Urho3D::Text* text;
};

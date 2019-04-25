#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>

class TopInfoPanel : public AbstractWindowPanel
{
public:
	explicit TopInfoPanel(Urho3D::XMLFile* _style);
	~TopInfoPanel();
	
	void hoverOn();
	void click();
	void hoverOff();

private:
	//Urho3D::String createMessage(HudData* hudData);
	void createBody() override;

	bool hoverIsOn = false;
	Urho3D::Text* text;
};


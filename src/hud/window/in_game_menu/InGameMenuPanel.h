#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/Button.h>

class InGameMenuPanel : public AbstractWindowPanel
{
public:
	InGameMenuPanel(Urho3D::XMLFile* _style);
	~InGameMenuPanel();
private:
	void createBody() override;
	Urho3D::Button* toggleButton;
};


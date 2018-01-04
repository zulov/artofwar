#pragma once
#include "hud/window/AbstractWindowPanel.h"

class MainMenuDetailsPanel :public AbstractWindowPanel
{
public:
	MainMenuDetailsPanel(Urho3D::XMLFile* _style);
	~MainMenuDetailsPanel();
private:
	void createBody() override;
};


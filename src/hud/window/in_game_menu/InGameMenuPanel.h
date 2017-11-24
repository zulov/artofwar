#pragma once
#include "hud/window/AbstractWindowPanel.h"

class InGameMenuPanel : public AbstractWindowPanel
{
public:
	InGameMenuPanel(Urho3D::XMLFile* _style);
	~InGameMenuPanel();
private:
	void createBody() override;
};


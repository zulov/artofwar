#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/Button.h>
#include "hud/HudElement.h"
#include <vector>

class InGameMenuPanel : public AbstractWindowPanel
{
public:
	InGameMenuPanel(Urho3D::XMLFile* _style);
	~InGameMenuPanel();
	void setVisible(bool enable) override;
private:
	void createBody() override;
	Urho3D::Button* toggleButton;
	std::vector<HudElement*>* buttons;
};


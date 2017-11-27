#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/Button.h>
#include "hud/HudElement.h"
#include <vector>
#include "hud/window/middle/AbstractMiddlePanel.h"

class InGameMenuPanel : public AbstractWindowPanel
{
public:
	InGameMenuPanel(Urho3D::XMLFile* _style);
	~InGameMenuPanel();
	void setVisible(bool enable) override;
	Urho3D::Button* getToggleButton();
	std::vector<HudElement*>* getButtons();
	std::vector<HudElement*>* getClosedButtons();
	void toggle();
	void action(short id);
	void close();
private:
	void createBody() override;
	bool menuVisibility = false;
	Urho3D::Button* toggleButton;
	std::vector<HudElement*>* buttons;
	AbstractMiddlePanel** addionalPanels;
};


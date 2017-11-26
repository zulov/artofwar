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
	Urho3D::Button* getToggleButton();
	Urho3D::Button* getCloseButton();
	std::vector<HudElement*>* getButtons();
	void toggle();
	void action(short id);
	void close();
private:
	void createBody() override;
	bool menuVisibility = false;
	Urho3D::Button* toggleButton;
	std::vector<HudElement*>* buttons;
	Urho3D::Window * additionalWindow;
	Urho3D::Button* buttonClose;
};


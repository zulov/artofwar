#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include "MainMenuDetailsPanel.h"
#include <vector>
#include "hud/HudElement.h"

#define MAIN_MENU_BUTTON_NUMBER 5

class MainMenuPanel : public AbstractWindowPanel
{
public:
	MainMenuPanel(Urho3D::XMLFile* _style);
	~MainMenuPanel();
	void action(short id);
	void close();
	void setVisible(bool enable) override;
	void HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
private:
	void createBody() override;
	std::vector<HudElement*> buttons;
	MainMenuDetailsPanel** detailsPanels;
};

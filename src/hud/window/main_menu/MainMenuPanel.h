#pragma once
#include "MainMenuDetailsPanel.h"
#include "hud/HudData.h"
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/Button.h>

#define MAIN_MENU_BUTTON_NUMBER 5

class MainMenuPanel : public AbstractWindowPanel
{
public:
	MainMenuPanel(Urho3D::XMLFile* _style);
	~MainMenuPanel();
	void action(short id);
	void close();
	void setVisible(bool enable) override;
	Urho3D::Button* getNewGameProceed();
	Urho3D::Button* getLoadButton();
	Urho3D::Button* getCloseButton();
	void HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
private:
	void createBody() override;
	MainMenuDetailsPanel** detailsPanels;
	Urho3D::BorderImage* background;
};

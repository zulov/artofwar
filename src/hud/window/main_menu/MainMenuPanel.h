#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/Button.h>


#define MAIN_MENU_BUTTON_NUMBER 5

class MainMenuDetailsPanel;

class MainMenuPanel : public AbstractWindowPanel
{
public:
	explicit MainMenuPanel(Urho3D::XMLFile* _style);
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

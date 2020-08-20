#pragma once
#include <Urho3D/UI/Button.h>
#include "hud/window/AbstractWindowPanel.h"

constexpr char MAIN_MENU_BUTTON_NUMBER = 5;

class MainMenuDetailsPanel;

class MainMenuPanel : public AbstractWindowPanel {
public:
	explicit MainMenuPanel(Urho3D::XMLFile* _style);
	~MainMenuPanel();
	void action(short id) const;
	void close() const;
	void setVisible(bool enable) override;
	Urho3D::Button* getNewGameProceed() const;
	Urho3D::Button* getLoadButton() const;
	Urho3D::Button* getCloseButton() const;
	void HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
private:
	void createBody() override;
	MainMenuDetailsPanel* detailsPanels[MAIN_MENU_BUTTON_NUMBER];
	Urho3D::BorderImage* background;
};

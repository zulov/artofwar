#pragma once
#include <Urho3D/UI/Button.h>
#include "hud/window/AbstractWindowPanel.h"

#define IN_GAME_MENU_BUTTON_NUMBER 5

class AbstractMiddlePanel;

class InGameMenuPanel : public AbstractWindowPanel {
public:
	explicit InGameMenuPanel(Urho3D::XMLFile* _style);
	~InGameMenuPanel();
	void setVisible(bool enable) override;
	Urho3D::Button* getSaveButton() const;

private:
	void toggle();
	void action(short id);
	void close() const;
	void createBody() override;
	void HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleToggle(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	bool menuVisibility = false;
	Urho3D::Button* toggleButton;
	AbstractMiddlePanel* additionalPanels[IN_GAME_MENU_BUTTON_NUMBER];
};

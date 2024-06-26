#pragma once
#include "hud/window/EventPanel.h"

constexpr char IN_GAME_MENU_BUTTON_NUMBER = 5;

namespace Urho3D
{
	class UIElement;
	class Button;
}

class AbstractMiddlePanel;

class InGameMenuPanel : public EventPanel {
public:
	explicit InGameMenuPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style);
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

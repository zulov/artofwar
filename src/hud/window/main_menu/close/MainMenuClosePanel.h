#pragma once
#include "hud/window/main_menu/MainMenuDetailsPanel.h"

namespace Urho3D {
	class UIElement;
	class Text;
	class Button;
}

class MainMenuClosePanel : public MainMenuDetailsPanel {
public:
	MainMenuClosePanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style, const Urho3D::String& _title);
	~MainMenuClosePanel() = default;
	Urho3D::Button* getCloseButton() const;
private:
	void createBody() override;
	void HandleCancelClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	Urho3D::Window* miniWindow;
	Urho3D::Text* text;
	Urho3D::UIElement* row;
	Urho3D::Button* yes;
	Urho3D::Button* no;
};

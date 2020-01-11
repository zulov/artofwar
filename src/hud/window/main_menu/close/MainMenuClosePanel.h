#pragma once
#include "hud/window/main_menu/MainMenuDetailsPanel.h"
#include <Urho3D/UI/Button.h>

namespace Urho3D {
	class Button;
}

class MainMenuClosePanel : public MainMenuDetailsPanel
{
public:
	MainMenuClosePanel(Urho3D::XMLFile* _style, const Urho3D::String& _title);
	~MainMenuClosePanel();
	Urho3D::Button * getCloseButton() const;
private:
	void createBody() override;
	void HandleCancelClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	Urho3D::Window * miniWindow; 
	Urho3D::Text * text;
	Urho3D::UIElement* row;	
	Urho3D::Button * yes;
	Urho3D::Button * no;
};

#pragma once
#include "hud/window/main_menu/MainMenuDetailsPanel.h"

namespace Urho3D {
	class Button;
}

class MainMenuClosePanel : public MainMenuDetailsPanel
{
public:
	MainMenuClosePanel(Urho3D::XMLFile* _style, Urho3D::String _title);
	~MainMenuClosePanel();
private:
	void createBody() override;


	Urho3D::Window * miniWindow; 
	Urho3D::Text * text;
	Urho3D::UIElement* row;	
	Urho3D::Button * yes;
	Urho3D::Button * no;
};

#pragma once
#include "hud/window/main_menu/MainMenuDetailsPanel.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/ListView.h>


class MainMenuLoadPanel : public MainMenuDetailsPanel
{
public:
	MainMenuLoadPanel(Urho3D::XMLFile* _style, Urho3D::String _title);
	~MainMenuLoadPanel();
	void HandleLoadClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	Urho3D::Button * getLoadButton();
private:
	void createBody() override;
	void action(Urho3D::String saveName);
	Urho3D::UIElement* leftMock;
	Urho3D::ListView * list;
	Urho3D::ScrollView* content;

	Urho3D::Button* loadButton;
};

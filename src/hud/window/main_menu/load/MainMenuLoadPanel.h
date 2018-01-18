#pragma once
#include "hud/window/main_menu/MainMenuDetailsPanel.h"
#include <Urho3D/UI/ListView.h>
#include "hud/HudElement.h"
#include <vector>
#include <Urho3D/UI/Button.h>

class MainMenuLoadPanel : public MainMenuDetailsPanel
{
public:
	MainMenuLoadPanel(Urho3D::XMLFile* _style, Urho3D::String _title);
	~MainMenuLoadPanel();
	void HandleLoadClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
private:
	void createBody() override;
	void action(short id);
	Urho3D::UIElement* leftMock;
	Urho3D::ListView * list;
	Urho3D::ScrollView* content;

	std::vector<HudElement*> listElements;
	Urho3D::Button* loadButton;
};

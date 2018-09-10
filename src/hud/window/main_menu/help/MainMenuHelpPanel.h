#pragma once
#include "hud/window/main_menu/MainMenuDetailsPanel.h"
#include <Urho3D/UI/ListView.h>


#define HELP_ITEMS 5
class MainMenuHelpPanel : public MainMenuDetailsPanel
{
public:
	MainMenuHelpPanel(Urho3D::XMLFile* _style, const Urho3D::String& _title);
	~MainMenuHelpPanel();
	void action(short id);
	void HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
private:
	void createBody() override;

	Urho3D::ListView * list;
	Urho3D::ScrollView* content;
	Urho3D::Text* contentText;
};


#pragma once
#include <Urho3D/UI/ListView.h>
#include "hud/window/main_menu/MainMenuDetailsPanel.h"

constexpr char HELP_ITEMS = 5;

class MainMenuHelpPanel : public MainMenuDetailsPanel {
public:
	MainMenuHelpPanel(Urho3D::XMLFile* _style, const Urho3D::String& _title);
	~MainMenuHelpPanel() = default;
	void action(short id) const;
	void HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
private:
	void createBody() override;

	Urho3D::ListView* list;
	Urho3D::ScrollView* content;
	Urho3D::Text* contentText;
};

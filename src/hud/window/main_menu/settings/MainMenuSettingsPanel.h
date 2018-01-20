#pragma once
#include "hud/window/main_menu/MainMenuDetailsPanel.h"
#include <Urho3D/Resource/Localization.h>

#define SETTINGS_ROWS_NUMBER 8

class MainMenuSettingsPanel : public MainMenuDetailsPanel
{
public:
	MainMenuSettingsPanel(Urho3D::XMLFile* _style, Urho3D::String _title);
	~MainMenuSettingsPanel();
private:
	void createBody() override;
	void populateLabels(int index, Urho3D::String name);

	Urho3D::BorderImage* rows[SETTINGS_ROWS_NUMBER];
};

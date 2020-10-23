#pragma once

#include "hud/window/main_menu/MainMenuDetailsPanel.h"

namespace Urho3D {
	class BorderImage;
	class CheckBox;
	class DropDownList;
	class Button;
}

constexpr char SETTINGS_ROWS_NUMBER = 9;

struct SettingsForm;

class MainMenuSettingsPanel : public MainMenuDetailsPanel {
public:
	MainMenuSettingsPanel(Urho3D::XMLFile* _style, const Urho3D::String& _title);
	~MainMenuSettingsPanel();
	void setValues(int graphID) const;
private:
	void createBody() override;
	void populateLabels(int index, const Urho3D::String& name);
	void popualateForm(SettingsForm* form) const;
	void HandleChangeSettings(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void HandleSaveSettings(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

	Urho3D::BorderImage* rows[SETTINGS_ROWS_NUMBER];
	Urho3D::Button* save;

	Urho3D::DropDownList* settings;
	Urho3D::DropDownList* resolution;
	Urho3D::CheckBox* fullScreen;
	Urho3D::DropDownList* maxFps;
	Urho3D::DropDownList* minFps;
	Urho3D::CheckBox* vSync;
	Urho3D::DropDownList* textureQuality;
	Urho3D::CheckBox* shadow;
	Urho3D::DropDownList* hudSize;

	SettingsForm* data;
};

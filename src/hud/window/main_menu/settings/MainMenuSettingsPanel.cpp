#include "MainMenuSettingsPanel.h"
#include <Urho3D/Resource/Localization.h>
#include "Game.h"
#include "hud/UiUtils.h"


MainMenuSettingsPanel::
MainMenuSettingsPanel(Urho3D::XMLFile* _style, Urho3D::String _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuSettingsMock";
}

MainMenuSettingsPanel::~MainMenuSettingsPanel() = default;

void MainMenuSettingsPanel::createBody() {
	MainMenuDetailsPanel::createBody();
	for (int i = 0; i < SETTINGS_ROWS_NUMBER; ++i) {
		rows[i] = body->CreateChild<Urho3D::BorderImage>();
		rows[i]->SetStyle("MainMenuSettingsRow");
	}

	populateLabels(0, "mmsp_settings");
	populateLabels(1, "mmsp_resolution");
	populateLabels(2, "mmsp_fullscreen");
	populateLabels(3, "mmsp_max_fps");
	populateLabels(4, "mmsp_min_fps");
	populateLabels(5, "mmsp_vSync");
	populateLabels(6, "mmsp_texture_quality");
	populateLabels(7, "mmsp_shadow");

	settings = createDropDownList(rows[0], "MainMenuNewGameDropDownList", style);
	//addTextItems(gameSpeed, {l10n->Get("slow"), l10n->Get("normal"), l10n->Get("fast")}, style);
	resolution = createDropDownList(rows[1], "MainMenuNewGameDropDownList", style);


	maxFps = createDropDownList(rows[3], "MainMenuNewGameDropDownList", style);
	minFps = createDropDownList(rows[4], "MainMenuNewGameDropDownList", style);

	textureQuality = createDropDownList(rows[6], "MainMenuNewGameDropDownList", style);
	shadow = createDropDownList(rows[7], "MainMenuNewGameDropDownList", style);

	Urho3D::Localization* l10n = Game::get()->getLocalization();

	save = body->CreateChild<Urho3D::Button>();
	save->SetStyle("MainMenuSettingsButton", style);
	addChildText(save, "MainMenuSettingsButtonText", l10n->Get("mmsp_save"), style);
}

void MainMenuSettingsPanel::populateLabels(int index, Urho3D::String name) {
	Urho3D::Localization* l10n = Game::get()->getLocalization();
	Urho3D::Text* text = rows[index]->CreateChild<Urho3D::Text>();
	text->SetStyle("MainMenuSettingsLabel");
	text->SetText(l10n->Get(name));
}

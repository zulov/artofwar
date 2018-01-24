#include "MainMenuSettingsPanel.h"
#include <Urho3D/Resource/Localization.h>
#include "Game.h"
#include "hud/UiUtils.h"
#include "database/DatabaseCache.h"


MainMenuSettingsPanel::
MainMenuSettingsPanel(Urho3D::XMLFile* _style, Urho3D::String _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuSettingsMock";
}

MainMenuSettingsPanel::~MainMenuSettingsPanel() {
	delete data;
};

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
	populateLabels(8, "mmsp_hud_size");


	Urho3D::Localization* l10n = Game::get()->getLocalization();


	settings = createDropDownList(rows[0], "MainMenuNewGameDropDownList", style);
	std::vector<String> settingsNames;
	for (int i = 0; i < Game::get()->getDatabaseCache()->getGraphSettingsSize(); ++i) {
		db_graph_settings* settings = Game::get()->getDatabaseCache()->getGraphSettings(i);
		settingsNames.push_back(l10n->Get(settings->name));
	}
	addChildTexts(settings, settingsNames, style);

	resolution = createDropDownList(rows[1], "MainMenuNewGameDropDownList", style);
	addChildTexts(resolution, {"1366x768", "1600x900", "1920x1080", "2560x1440", "4096x2160"}, style);
	
	//TODO zrobic z tym tabelke

	fullScreen = rows[2]->CreateChild<CheckBox>();
	fullScreen->SetStyle("CheckBox", style);

	maxFps = createDropDownList(rows[3], "MainMenuNewGameDropDownList", style);
	addChildTexts(maxFps, {"30", "60", "120", "240"}, style, {30, 60, 120, 240}, "IntValue");

	minFps = createDropDownList(rows[4], "MainMenuNewGameDropDownList", style);
	addChildTexts(minFps, {"1", "5", "10"}, style, {1, 5, 10}, "IntValue");

	vSync = rows[5]->CreateChild<CheckBox>();
	vSync->SetStyle("CheckBox", style);

	textureQuality = createDropDownList(rows[6], "MainMenuNewGameDropDownList", style);
	addChildTexts(textureQuality, {l10n->Get("low"), l10n->Get("normal"), l10n->Get("high")}, style);

	shadow = rows[7]->CreateChild<CheckBox>();
	shadow->SetStyle("CheckBox", style);

	hudSize = createDropDownList(rows[8], "MainMenuNewGameDropDownList", style);

	std::vector<String> hudNames;
	for (int i = 0; i < Game::get()->getDatabaseCache()->getHudSizeSize(); ++i) {
		db_hud_size* hudSize = Game::get()->getDatabaseCache()->getHudSize(i);
		hudNames.push_back(hudSize->name);
	}
	addChildTexts(hudSize, hudNames, style);

	save = body->CreateChild<Urho3D::Button>();
	save->SetStyle("MainMenuSettingsButton", style);
	addChildText(save, "MainMenuSettingsButtonText", l10n->Get("mmsp_save"), style);
	data = new SettingsForm();
	save->SetVar("SettingsForm", data);
}

void MainMenuSettingsPanel::populateLabels(int index, Urho3D::String name) {
	Urho3D::Localization* l10n = Game::get()->getLocalization();
	Urho3D::Text* text = rows[index]->CreateChild<Urho3D::Text>();
	text->SetStyle("MainMenuSettingsLabel");
	text->SetText(l10n->Get(name));
}

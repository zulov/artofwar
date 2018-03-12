#include "MainMenuSettingsPanel.h"
#include <Urho3D/Resource/Localization.h>
#include "Game.h"
#include "hud/UiUtils.h"
#include "database/DatabaseCache.h"
#include <Urho3D/UI/UIEvents.h>


MainMenuSettingsPanel::
MainMenuSettingsPanel(Urho3D::XMLFile* _style, Urho3D::String _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuSettingsMock";
}

MainMenuSettingsPanel::~MainMenuSettingsPanel() {
	delete data;
}

void MainMenuSettingsPanel::setValues(int graphID) {
	db_graph_settings* graphSettings = Game::get()->getDatabaseCache()->getGraphSettings(graphID);

	fullScreen->SetChecked(graphSettings->fullscreen);
	vSync->SetChecked(graphSettings->v_sync);
	textureQuality->SetSelection(graphSettings->texture_quality);
	shadow->SetChecked(graphSettings->shadow);
	hudSize->SetSelection(graphSettings->hud_size);
}

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
	SubscribeToEvent(settings, E_ITEMSELECTED, URHO3D_HANDLER(MainMenuSettingsPanel, HandleChangeSettings));

	addChildTexts(settings, settingsNames, style);

	resolution = createDropDownList(rows[1], "MainMenuNewGameDropDownList", style);
	std::vector<String> resNames;
	for (int i = 0; i < Game::get()->getDatabaseCache()->getResourceSize(); ++i) {
		db_resolution* res = Game::get()->getDatabaseCache()->getResolution(i);
		resNames.push_back(String(res->x) + "x" + String(res->y));
	}
	addChildTexts(resolution, resNames, style);

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
	SubscribeToEvent(save, E_CLICK, URHO3D_HANDLER(MainMenuSettingsPanel, HandleSaveSettings));

	db_settings* set = Game::get()->getDatabaseCache()->getSettings();
	settings->SetSelection(set->graph);
	resolution->SetSelection(set->resolution);
	int graphID = set->graph;
	setValues(graphID);
}

void MainMenuSettingsPanel::populateLabels(int index, Urho3D::String name) {
	Urho3D::Localization* l10n = Game::get()->getLocalization();
	Urho3D::Text* text = rows[index]->CreateChild<Urho3D::Text>();
	text->SetStyle("MainMenuSettingsLabel");
	text->SetText(l10n->Get(name));
}

void MainMenuSettingsPanel::popualateForm(SettingsForm* form) {
	form->resolution = resolution->GetSelection();
	form->fullScreen = fullScreen->IsChecked();
	form->maxFps = maxFps->GetSelectedItem()->GetVar("IntValue").GetInt();
	form->minFps = minFps->GetSelectedItem()->GetVar("IntValue").GetInt();
	form->vSync = vSync->IsChecked();
	form->textureQuality = textureQuality->GetSelection();
	form->shadow = shadow->IsChecked();
	form->hudSize = hudSize->GetSelection();
}

void MainMenuSettingsPanel::HandleChangeSettings(StringHash eventType, VariantMap& eventData) {
	DropDownList* element = static_cast<DropDownList*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	unsigned index = element->GetSelection();
	if (index > 0) {
		setValues(index);
	}
}

void MainMenuSettingsPanel::HandleSaveSettings(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	SettingsForm* form = static_cast<SettingsForm *>(element->GetVar("SettingsForm").GetVoidPtr());

	popualateForm(form);

	db_graph_settings* graphSettings = new db_graph_settings(0, form->hudSize, nullptr, form->fullScreen, form->maxFps,
	                                                         form->minFps, nullptr, form->vSync, form->shadow,
	                                                         form->textureQuality);
	Game::get()->getDatabaseCache()->setGraphSettings(0, graphSettings);
	db_settings* settings = new db_settings(0, form->resolution);
	Game::get()->getDatabaseCache()->setSettings(0, settings);
}

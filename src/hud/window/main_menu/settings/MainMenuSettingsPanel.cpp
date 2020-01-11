#include "MainMenuSettingsPanel.h"
#include "SettingsForm.h"
#include "database/DatabaseCache.h"
#include "hud/UiUtils.h"
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/UIEvents.h>


MainMenuSettingsPanel::
MainMenuSettingsPanel(Urho3D::XMLFile* _style, Urho3D::String _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuSettingsMock";
}

MainMenuSettingsPanel::~MainMenuSettingsPanel() {
	delete data;
}

void MainMenuSettingsPanel::setValues(int graphID) const {
	db_graph_settings* graphSettings = Game::getDatabaseCache()->getGraphSettings(graphID);

	fullScreen->SetChecked(graphSettings->fullscreen);
	vSync->SetChecked(graphSettings->v_sync);
	textureQuality->SetSelection(graphSettings->texture_quality);
	shadow->SetChecked(graphSettings->shadow);
	hudSize->SetSelection(graphSettings->hud_size);
}

void MainMenuSettingsPanel::createBody() {
	MainMenuDetailsPanel::createBody();
	for (auto& row : rows) {
		row = createElement<Urho3D::BorderImage>(body, style,"MainMenuSettingsRow");
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

	Urho3D::Localization* l10n = Game::getLocalization();

	settings = createElement<Urho3D::DropDownList>(rows[0], style, "MainMenuNewGameDropDownList");
	std::vector<Urho3D::String> settingsNames;
	for (int i = 0; i < Game::getDatabaseCache()->getGraphSettingsSize(); ++i) {
		db_graph_settings* settings = Game::getDatabaseCache()->getGraphSettings(i);
		settingsNames.push_back(l10n->Get(settings->name));
	}
	SubscribeToEvent(settings, Urho3D::E_ITEMSELECTED, URHO3D_HANDLER(MainMenuSettingsPanel, HandleChangeSettings));

	addChildTexts(settings, settingsNames, style);

	resolution =  createElement<Urho3D::DropDownList>(rows[1], style, "MainMenuNewGameDropDownList");
	std::vector<Urho3D::String> resNames;
	for (int i = 0; i < Game::getDatabaseCache()->getResourceSize(); ++i) {
		db_resolution* res = Game::getDatabaseCache()->getResolution(i);
		resNames.push_back(Urho3D::String(res->x) + "x" + Urho3D::String(res->y));
	}
	addChildTexts(resolution, resNames, style);

	fullScreen = createElement<Urho3D::CheckBox>(rows[2], style, "CheckBox");

	maxFps = createElement<Urho3D::DropDownList>(rows[3], style, "MainMenuNewGameDropDownList");
	addChildTexts(maxFps, {"30", "60", "120", "240"}, style, {30, 60, 120, 240}, "IntValue");

	minFps = createElement<Urho3D::DropDownList>(rows[4], style, "MainMenuNewGameDropDownList");
	addChildTexts(minFps, {"1", "5", "10"}, style, {1, 5, 10}, "IntValue");

	vSync = createElement<Urho3D::CheckBox>(rows[5], style, "CheckBox");

	textureQuality = createElement<Urho3D::DropDownList>(rows[6], style, "MainMenuNewGameDropDownList");
	addChildTexts(textureQuality, {l10n->Get("low"), l10n->Get("normal"), l10n->Get("high")}, style);

	shadow = createElement<Urho3D::CheckBox>(rows[7], style, "CheckBox");

	hudSize = createElement<Urho3D::DropDownList>(rows[8], style, "MainMenuNewGameDropDownList");

	std::vector<Urho3D::String> hudNames;
	for (int i = 0; i < Game::getDatabaseCache()->getHudSizeSize(); ++i) {
		db_hud_size* hudSize = Game::getDatabaseCache()->getHudSize(i);
		hudNames.push_back(hudSize->name);
	}
	addChildTexts(hudSize, hudNames, style);

	save = createElement<Urho3D::Button>(body, style,"MainMenuSettingsButton");
	addChildText(save, "MainMenuSettingsButtonText", l10n->Get("mmsp_save"), style);
	data = new SettingsForm();
	save->SetVar("SettingsForm", data);
	SubscribeToEvent(save, Urho3D::E_CLICK, URHO3D_HANDLER(MainMenuSettingsPanel, HandleSaveSettings));

	db_settings* set = Game::getDatabaseCache()->getSettings();
	settings->SetSelection(set->graph);
	resolution->SetSelection(set->resolution);
	int graphID = set->graph;
	setValues(graphID);
}

void MainMenuSettingsPanel::populateLabels(int index, Urho3D::String name) {
	addChildText(rows[index], "MainMenuSettingsLabel", Game::getLocalization()->Get(name), style);
}

void MainMenuSettingsPanel::popualateForm(SettingsForm* form) const {
	form->resolution = resolution->GetSelection();
	form->fullScreen = fullScreen->IsChecked();
	form->maxFps = maxFps->GetSelectedItem()->GetVar("IntValue").GetInt();
	form->minFps = minFps->GetSelectedItem()->GetVar("IntValue").GetInt();
	form->vSync = vSync->IsChecked();
	form->textureQuality = textureQuality->GetSelection();
	form->shadow = shadow->IsChecked();
	form->hudSize = hudSize->GetSelection();
}

void MainMenuSettingsPanel::HandleChangeSettings(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::DropDownList*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	unsigned index = element->GetSelection();
	if (index > 0) {
		setValues(index);
	}
}

void MainMenuSettingsPanel::HandleSaveSettings(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto form = SettingsForm::getFromElement(eventData);

	popualateForm(form);

	auto graphSettings = new db_graph_settings(0, form->hudSize, nullptr, form->fullScreen, form->maxFps,
	                                                         form->minFps, nullptr, form->vSync, form->shadow,
	                                                         form->textureQuality);
	Game::getDatabaseCache()->setGraphSettings(0, graphSettings);
	Game::getDatabaseCache()->setSettings(0, new db_settings(0, form->resolution));
}

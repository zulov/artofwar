#include "MainMenuHelpPanel.h"
#include "hud/UiUtils.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UIEvents.h>


MainMenuHelpPanel::
MainMenuHelpPanel(Urho3D::XMLFile* _style, Urho3D::String _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuHelpMock";
}

void MainMenuHelpPanel::createBody() {
	MainMenuDetailsPanel::createBody();
	list = window->CreateChild<Urho3D::ListView>();
	list->SetStyle("HelpList", style);

	for (int i = 0; i < HELP_ITEMS; ++i) {

		auto button = simpleButton(nullptr, style, "HelpListButton");
		addChildText(button, "HelpListText", Game::getLocalization()->Get("help_key_" + Urho3D::String(i)), style);

		list->AddItem(button);

		button->SetVar("Num", i);
		SubscribeToEvent(button, Urho3D::E_CLICK, URHO3D_HANDLER(MainMenuHelpPanel, HandleButtonClick));
	}

	content = window->CreateChild<Urho3D::ScrollView>();
	content->SetStyle("HelpContent", style);

	contentText = addChildText(content, "HelpContentText", Game::getLocalization()->Get("mock"), style);

	contentText->SetWidth(content->GetWidth() * 0.9);
	content->AddChild(contentText);
}


MainMenuHelpPanel::~MainMenuHelpPanel() = default;

void MainMenuHelpPanel::action(short id) {
	contentText->SetText(Game::getLocalization()->Get("help_value_" + Urho3D::String(id)));
	contentText->SetWidth(content->GetWidth() * 0.9);
}

void MainMenuHelpPanel::HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	int id = element->GetVar("Num").GetInt();

	action(id);
}

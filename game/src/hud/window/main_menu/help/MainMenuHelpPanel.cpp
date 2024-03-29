#include "MainMenuHelpPanel.h"

#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include "hud/UiUtils.h"


MainMenuHelpPanel::
MainMenuHelpPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style, const Urho3D::String& _title): MainMenuDetailsPanel(root, _style, _title) {
	bodyStyle = "MainMenuHelpMock";
}

void MainMenuHelpPanel::createBody() {
	MainMenuDetailsPanel::createBody();
	list = createElement<Urho3D::ListView>(window, style, "HelpList");

	for (int i = 0; i < HELP_ITEMS; ++i) {
		auto button = simpleButton(style, "HelpListButton");
		addChildText(button, "HelpListText", Game::getLocalization()->Get("help_key_" + Urho3D::String(i)), style);

		list->AddItem(button);

		button->SetVar("Num", i);
		SubscribeToEvent(button, Urho3D::E_CLICK, URHO3D_HANDLER(MainMenuHelpPanel, HandleButtonClick));
	}

	content = createElement<Urho3D::ScrollView>(window, style, "HelpContent");

	contentText = addChildText(content, "HelpContentText", Game::getLocalization()->Get("mock"), style);

	contentText->SetWidth(content->GetWidth() * 0.9);
}

void MainMenuHelpPanel::action(short id) const {
	contentText->SetText(Game::getLocalization()->Get("help_value_" + Urho3D::String(id)));
	contentText->SetWidth(content->GetWidth() * 0.9);
}

void MainMenuHelpPanel::HandleButtonClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	auto element = static_cast<Urho3D::UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	int id = element->GetVar("Num").GetInt();

	action(id);
}

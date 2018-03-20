#include "MainMenuHelpPanel.h"
#include "Game.h"
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

		Button* button = simpleButton(nullptr, style, "HelpListButton");
		Urho3D::Text* element = button->CreateChild<Text>();

		element->SetText(Game::get()->getLocalization()->Get("help_key_" + String(i)));
		element->SetStyle("HelpListText");
		button->AddChild(element);
		list->AddItem(button);

		button->SetVar("Num", i);
		SubscribeToEvent(button, E_CLICK, URHO3D_HANDLER(MainMenuHelpPanel, HandleButtonClick));
	}

	content = window->CreateChild<Urho3D::ScrollView>();
	content->SetStyle("HelpContent", style);

	contentText = content->CreateChild<Urho3D::Text>();
	contentText->SetStyle("HelpContentText", style);
	contentText->SetText(Game::get()->getLocalization()->Get("mock"));

	contentText->SetWidth(content->GetWidth() * 0.9);
	content->AddChild(contentText);

}


MainMenuHelpPanel::~MainMenuHelpPanel() = default;

void MainMenuHelpPanel::action(short id) {
	contentText->SetText(Game::get()->getLocalization()->Get("help_value_" + String(id)));
	contentText->SetWidth(content->GetWidth() * 0.9);
}

void MainMenuHelpPanel::HandleButtonClick(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	int id = element->GetVar("Num").GetInt();

	action(id);
}

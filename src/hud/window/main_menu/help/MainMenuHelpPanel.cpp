#include "MainMenuHelpPanel.h"
#include "Game.h"
#include "utils.h"
#include "hud/UiUtils.h"
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UIEvents.h>


MainMenuHelpPanel::
MainMenuHelpPanel( Urho3D::String _title): MainMenuDetailsPanel( _title) {
	bodyStyle = "MainMenuHelpMock";
}

void MainMenuHelpPanel::createBody() {
	MainMenuDetailsPanel::createBody();
	list = window->CreateChild<Urho3D::ListView>();
	list->SetStyle("HelpList");

	for (int i = 0; i < HELP_ITEMS; ++i) {

		Button* button = simpleButton(nullptr, "HelpListButton");
		Urho3D::Text* element = button->CreateChild<Text>();

		HudElement* hudElement = new HudElement(button);
		hudElement->setId(i, ENTITY);
		element->SetText(Game::get()->getLocalization()->Get("help_key_" + String(i)));
		element->SetStyle("HelpListText");
		button->AddChild(element);
		list->AddItem(button);
		listElements.push_back(hudElement);
		button->SetVar("HudElement", hudElement);
		SubscribeToEvent(button, E_CLICK, URHO3D_HANDLER(MainMenuHelpPanel, HandleButtonClick));
	}

	content = window->CreateChild<Urho3D::ScrollView>();
	content->SetStyle("HelpContent");

	contentText = content->CreateChild<Urho3D::Text>();
	contentText->SetStyle("HelpContentText");
	contentText->SetText(Game::get()->getLocalization()->Get("mock"));

	contentText->SetWidth(content->GetWidth() * 0.9);
	content->AddChild(contentText);

}


MainMenuHelpPanel::~MainMenuHelpPanel() {
	clear_vector(listElements);
}

void MainMenuHelpPanel::action(short id) {
	contentText->SetText(Game::get()->getLocalization()->Get("help_value_" + String(id)));
	contentText->SetWidth(content->GetWidth() * 0.9);
}

void MainMenuHelpPanel::HandleButtonClick(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	HudElement* hudElement = static_cast<HudElement *>(element->GetVar("HudElement").GetVoidPtr());

	action(hudElement->getId());
}

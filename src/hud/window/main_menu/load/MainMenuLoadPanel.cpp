#include "MainMenuLoadPanel.h"
#include "hud/UiUtils.h"
#include <Urho3D/UI/UIEvents.h>


namespace Urho3D {
	class Button;
}

MainMenuLoadPanel::
MainMenuLoadPanel(Urho3D::XMLFile* _style, Urho3D::String _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuLoadMock";
}

void MainMenuLoadPanel::createBody() {
	MainMenuDetailsPanel::createBody();
	leftMock = window->CreateChild<Urho3D::UIElement>();
	leftMock->SetStyle("LoadLeftMock", style);
	list = leftMock->CreateChild<Urho3D::ListView>();
	list->SetStyle("LoadList", style);
	int savesNumber = 5;
	for (int i = 0; i < savesNumber; ++i) {

		Urho3D::Button* button = simpleButton(nullptr, style, "LoadListButton");
		Urho3D::Text* element = button->CreateChild<Text>();

		HudElement* hudElement = new HudElement(button);
		hudElement->setId(i, ENTITY);
		element->SetText(Game::get()->getLocalization()->Get("help_key_" + String(i)));
		element->SetStyle("LoadListText");
		button->AddChild(element);
		list->AddItem(button);
		listElements.push_back(hudElement);
		button->SetVar("HudElement", hudElement);
		SubscribeToEvent(button, E_CLICK, URHO3D_HANDLER(MainMenuLoadPanel, HandleLoadClick));
	}

	content = window->CreateChild<Urho3D::ScrollView>();
	content->SetStyle("LoadContent", style);
	loadButton = leftMock->CreateChild<Urho3D::Button>();
	loadButton->SetStyle("LoadButton", style);
	addChildText(loadButton, "LoadButtonText", Game::get()->getLocalization()->Get("load"), style);
}

void MainMenuLoadPanel::action(short id) {
}


MainMenuLoadPanel::~MainMenuLoadPanel() {
}

void MainMenuLoadPanel::HandleLoadClick(StringHash eventType, VariantMap& eventData) {
	UIElement* element = static_cast<UIElement*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	HudElement* hudElement = static_cast<HudElement *>(element->GetVar("HudElement").GetVoidPtr());

	action(hudElement->getId());
}

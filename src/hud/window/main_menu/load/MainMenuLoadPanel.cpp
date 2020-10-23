#include "MainMenuLoadPanel.h"
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include "hud/UiUtils.h"


MainMenuLoadPanel::
MainMenuLoadPanel(Urho3D::XMLFile* _style, const Urho3D::String& _title): MainMenuDetailsPanel(_style, _title) {
	bodyStyle = "MainMenuLoadMock";
}

void MainMenuLoadPanel::createBody() {
	MainMenuDetailsPanel::createBody();
	leftMock = createElement<Urho3D::UIElement>(window, style,"LoadLeftMock"); 
	list = createElement<Urho3D::ListView>(window, style,"LoadList");

	Urho3D::Vector<Urho3D::String> files;
	GetSubsystem<Urho3D::FileSystem>()->ScanDir(files, "saves", "*.db", Urho3D::SCAN_FILES, false);

	for (const auto& name : files) {
		auto button = simpleButton(style, "LoadListButton");
		addChildText(button, "LoadListText", name, style);
		
		list->AddItem(button);

		SubscribeToEvent(button, Urho3D::E_CLICK, URHO3D_HANDLER(MainMenuLoadPanel, HandleLoadClick));
	}

	content = createElement<Urho3D::ScrollView>(window, style,"LoadContent");
	loadButton = createElement<Urho3D::Button>(leftMock, style,"LoadButton");

	addChildText(loadButton, "LoadButtonText", Game::getLocalization()->Get("load"), style);
}

void MainMenuLoadPanel::action(const Urho3D::String& saveName) {
}

void MainMenuLoadPanel::HandleLoadClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	const auto element = static_cast<Urho3D::Button*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	const auto text = dynamic_cast<Urho3D::Text*>(element->GetChild(0));

	loadButton->SetVar("LoadFileName", text->GetText());
	action(text->GetText());
}

Urho3D::Button* MainMenuLoadPanel::getLoadButton() const {
	return loadButton;
}

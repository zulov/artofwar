#include "MainMenuLoadPanel.h"
#include "hud/UiUtils.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/FileSystem.h>
#include <iostream>


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

	FileSystem* fileSystem = GetSubsystem<FileSystem>();

	Vector<String> files;
	fileSystem->ScanDir(files, "saves", "*.db", SCAN_FILES, false);

	for (auto name : files) {
		Urho3D::Button* button = simpleButton(nullptr, style, "LoadListButton");
		Urho3D::Text* element = button->CreateChild<Text>();
		
		element->SetText(name);
		element->SetStyle("LoadListText");
		button->AddChild(element);
		list->AddItem(button);

		SubscribeToEvent(button, E_CLICK, URHO3D_HANDLER(MainMenuLoadPanel, HandleLoadClick));
	}

	content = window->CreateChild<Urho3D::ScrollView>();
	content->SetStyle("LoadContent", style);
	loadButton = leftMock->CreateChild<Urho3D::Button>();
	loadButton->SetStyle("LoadButton", style);
	addChildText(loadButton, "LoadButtonText", Game::get()->getLocalization()->Get("load"), style);
}

void MainMenuLoadPanel::action(String saveName) {
}


MainMenuLoadPanel::~MainMenuLoadPanel() = default;

void MainMenuLoadPanel::HandleLoadClick(StringHash eventType, VariantMap& eventData) {
	Button* element = static_cast<Button*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	Text* text = static_cast<Text*>(element->GetChild(0));
	std::cout<<text->GetText().CString();
	loadButton->SetVar("LoadFileName", text->GetText());
	action(text->GetText());
}

Urho3D::Button* MainMenuLoadPanel::getLoadButton() {
	return loadButton;
}

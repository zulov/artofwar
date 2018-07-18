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

	auto fileSystem = GetSubsystem<Urho3D::FileSystem>();

	Urho3D::Vector<Urho3D::String> files;
	fileSystem->ScanDir(files, "saves", "*.db", Urho3D::SCAN_FILES, false);

	for (const auto& name : files) {
		auto button = simpleButton(nullptr, style, "LoadListButton");
		auto element = button->CreateChild<Urho3D::Text>();
		
		element->SetText(name);
		element->SetStyle("LoadListText");
		button->AddChild(element);
		list->AddItem(button);

		SubscribeToEvent(button, Urho3D::E_CLICK, URHO3D_HANDLER(MainMenuLoadPanel, HandleLoadClick));
	}

	content = window->CreateChild<Urho3D::ScrollView>();
	content->SetStyle("LoadContent", style);
	loadButton = leftMock->CreateChild<Urho3D::Button>();
	loadButton->SetStyle("LoadButton", style);
	addChildText(loadButton, "LoadButtonText", Game::getLocalization()->Get("load"), style);
}

void MainMenuLoadPanel::action(Urho3D::String saveName) {
}


MainMenuLoadPanel::~MainMenuLoadPanel() = default;

void MainMenuLoadPanel::HandleLoadClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	const auto element = static_cast<Urho3D::Button*>(eventData[Urho3D::UIMouseClick::P_ELEMENT].GetVoidPtr());
	const auto text = static_cast<Urho3D::Text*>(element->GetChild(0));

	loadButton->SetVar("LoadFileName", text->GetText());
	action(text->GetText());
}

Urho3D::Button* MainMenuLoadPanel::getLoadButton() {
	return loadButton;
}

#include "FilePanel.h"
#include "FileFormData.h"
#include "Game.h"
#include "hud/HudData.h"
#include <Urho3D/UI/UIEvents.h>


FilePanel::FilePanel(Urho3D::XMLFile* _style, const Urho3D::String& _title): AbstractMiddlePanel(_style, _title) {
	data = nullptr;
}


FilePanel::~FilePanel() {
	delete data;
}

void FilePanel::createBody() {
	AbstractMiddlePanel::createBody();
	const auto name = Game::getLocalization()->Get("save");
	lineEdit = body->CreateChild<Urho3D::LineEdit>();
	lineEdit->SetStyle("FileNameLineEdit", style);

	SubscribeToEvent(lineEdit, Urho3D::E_DEFOCUSED, URHO3D_HANDLER(FilePanel, HandleValueChange));

	action = body->CreateChild<Urho3D::Button>();
	action->SetStyle("FileConfirmButton", style);
	data = new FileFormData();
	action->SetVar("file_data", data);

	auto textInButton = action->CreateChild<Urho3D::Text>();
	textInButton->SetStyle("MiddleText", style);
	textInButton->SetText(name);

	//	list = body->CreateChild<Urho3D::DropDownList>();
	//	list->SetStyle("DropDownList", style);
	//	list->SetSize(128, 32);
	//	list->SetPosition(0, 128);
}

Urho3D::Button* FilePanel::getMainButton() {
	return action;
}

void FilePanel::HandleValueChange(const Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	data->fileName = Urho3D::String(lineEdit->GetText());
}
#include "FilePanel.h"
#include "FileFormData.h"
#include "Game.h"
#include "hud/UiUtils.h"
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/LineEdit.h>

FilePanel::FilePanel(Urho3D::XMLFile* _style, const Urho3D::String& _title): AbstractMiddlePanel(_style, _title) {
	data = nullptr;
}

FilePanel::~FilePanel() {
	delete data;
}

void FilePanel::createBody() {
	AbstractMiddlePanel::createBody();
	const auto name = Game::getLocalization()->Get("save");
	lineEdit = createElement<Urho3D::LineEdit>(body, style, "FileNameLineEdit");

	SubscribeToEvent(lineEdit, Urho3D::E_DEFOCUSED, URHO3D_HANDLER(FilePanel, HandleValueChange));

	action = createElement<Urho3D::Button>(body, style, "FileConfirmButton");
	data = new FileFormData();
	action->SetVar("file_data", data);

	addChildText(action, "MiddleText", name, style);
}

Urho3D::Button* FilePanel::getMainButton() {
	return action;
}

void FilePanel::HandleValueChange(const Urho3D::StringHash eventType, Urho3D::VariantMap& eventData) {
	data->fileName = Urho3D::String(lineEdit->GetText());
}

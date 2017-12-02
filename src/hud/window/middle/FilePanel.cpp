#include "FilePanel.h"
#include "Game.h"


FilePanel::FilePanel(Urho3D::XMLFile* _style, Urho3D::String _title): AbstractMiddlePanel(_style, _title) {
}


FilePanel::~FilePanel() {
}

void FilePanel::createBody() {
	AbstractMiddlePanel::createBody();
	Urho3D::String name = Game::get()->getLocalization()->Get("save");
	lineEdit = body->CreateChild<Urho3D::LineEdit>();
	lineEdit->SetStyle("FileNameLineEdit", style);
	lineEdit->SetPosition(0, 0);

	action = body->CreateChild<Urho3D::Button>();
	action->SetStyle("FileConfirmButton", style);
	action->SetPosition(0, 64);
	Urho3D::Text * textInButton = action->CreateChild<Urho3D::Text>();
	textInButton->SetStyle("MiddleText", style);
	textInButton->SetText(name);
	
//	list = body->CreateChild<Urho3D::DropDownList>();
//	list->SetStyle("DropDownList", style);
//	list->SetSize(128, 32);
//	list->SetPosition(0, 128);

}

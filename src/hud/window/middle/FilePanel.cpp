#include "FilePanel.h"


FilePanel::FilePanel(Urho3D::XMLFile* _style, Urho3D::String _title): AbstractMiddlePanel(_style, _title) {
}


FilePanel::~FilePanel() {
}

void FilePanel::createBody() {
	AbstractMiddlePanel::createBody();

	lineEdit = body->CreateChild<Urho3D::LineEdit>();
	lineEdit->SetStyle("LineEdit", style);
	lineEdit->SetSize(128, 32);
	lineEdit->SetPosition(0, 0);

	action = body->CreateChild<Urho3D::Button>();
	action->SetStyle("Button", style);
	action->SetSize(128, 32);
	action->SetPosition(0, 64);
	
	list = body->CreateChild<Urho3D::DropDownList>();
	list->SetStyle("DropDownList", style);
	list->SetSize(128, 32);
	list->SetPosition(0, 128);

}

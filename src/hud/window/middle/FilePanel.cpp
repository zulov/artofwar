#include "FilePanel.h"


FilePanel::FilePanel(Urho3D::XMLFile* _style): AbstractMiddlePanel(_style) {
}


FilePanel::~FilePanel() {
}

void FilePanel::createBody() {
	AbstractMiddlePanel::createBody();
//	fileSelector = window->CreateChild<Urho3D::FileSelector>();
//	fileSelector->SetDefaultStyle(style);

}

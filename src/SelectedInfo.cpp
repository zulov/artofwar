#include "SelectedInfo.h"


SelectedInfo::SelectedInfo() {
	message = new Urho3D::String();
	allNumber = 0;
}


SelectedInfo::~SelectedInfo() {
}

Urho3D::String* SelectedInfo::getMessage() {
	delete message;
	message = new Urho3D::String("Zaznaczeni: " + Urho3D::String(allNumber));
	return message;
}

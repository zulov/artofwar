#pragma once
#include "AbstractMiddlePanel.h"
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/DropDownList.h>
#include "FileFormData.h"

class FilePanel : public AbstractMiddlePanel
{
public:
	FilePanel(Urho3D::XMLFile* _style, Urho3D::String _title);
	~FilePanel();
	void createBody() override;
	Urho3D::Button* getMainButton() override;
	void HandleValueChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
private:
	Urho3D::LineEdit* lineEdit;
	Urho3D::DropDownList* list;
	Urho3D::Button* action;
	FileFormData * data;
};

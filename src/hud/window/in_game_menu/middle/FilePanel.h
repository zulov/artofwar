#pragma once
#include "AbstractMiddlePanel.h"

namespace Urho3D {
	class LineEdit;
}

struct FileFormData;

class FilePanel : public AbstractMiddlePanel {
public:
	FilePanel(Urho3D::XMLFile* _style, const Urho3D::String& _title);
	~FilePanel();
	void createBody() override;
	Urho3D::Button* getMainButton() override;
	void HandleValueChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
private:
	Urho3D::LineEdit* lineEdit;
	Urho3D::Button* action;
	FileFormData* data;
};

#pragma once
#include "AbstractMiddlePanel.h"
#include <Urho3D/UI/FileSelector.h>

class FilePanel : public AbstractMiddlePanel
{
public:
	FilePanel(Urho3D::XMLFile* _style);
	~FilePanel();
	void createBody() override;
private:
	//Urho3D::FileSelector* fileSelector;
};

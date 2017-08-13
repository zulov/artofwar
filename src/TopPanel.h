#pragma once
#include "AbstractWindowPanel.h"

class TopPanel :public AbstractWindowPanel
{
public:
	TopPanel(Urho3D::XMLFile* _style);
	~TopPanel();
private:
	void createBody() override;
};


#pragma once
#include "AbstractWindowPanel.h"

class MiniMapPanel :public AbstractWindowPanel
{
public:
	MiniMapPanel(Urho3D::XMLFile* _style);
	~MiniMapPanel();
private:
	void createBody() override;
};


#pragma once
#include "AbstractWindowPanel.h"
#include <vector>
#include "HudElement.h"

class UnitsPanel:public AbstractWindowPanel
{
public:
	UnitsPanel(Urho3D::XMLFile* _style, int _nation);
	~UnitsPanel();
	std::vector<HudElement*>*getButtons();
private:
	void createBody() override;
	std::vector<HudElement*>* buttons;
	int nation;
};


#pragma once
#include "../AbstractWindowPanel.h"
#include "hud/HudElement.h"
#include <vector>

class SelectedInfo;

class BuildPanel :public AbstractWindowPanel
{
public:
	BuildPanel(Urho3D::XMLFile* _style, int _nation);
	~BuildPanel();
	std::vector<HudElement*>*getButtons();
	void show();
private:
	void createBody() override;
	std::vector<HudElement*>* buttons;
	int nation;
};

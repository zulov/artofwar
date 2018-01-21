#pragma once
#include "../AbstractWindowPanel.h"
#include <vector>
#include "hud/HudElement.h"
#include "control/SelectedInfo.h"


class UnitsPanel : public AbstractWindowPanel
{
public:
	UnitsPanel();
	~UnitsPanel();
	std::vector<HudElement*>& getButtons();
	void show(SelectedInfo* selectedInfo);
private:
	void createBody() override;
	std::vector<HudElement*> buttons;
};

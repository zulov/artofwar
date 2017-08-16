#pragma once
#include "AbstractWindowPanel.h"
#include "HudElement.h"
#include <vector>
#include "SelectedInfo.h"

class OrdersPanel :public AbstractWindowPanel
{
public:
	OrdersPanel(Urho3D::XMLFile* _style);
	~OrdersPanel();
	void show(SelectedInfo* selectedInfo);
private:
	void createBody() override;
	std::vector<HudElement*>* buttons;
};


#pragma once
#include "AbstractWindowPanel.h"
#include "HudElement.h"
#include <vector>

class OrdersPanel :public AbstractWindowPanel
{
public:
	OrdersPanel(Urho3D::XMLFile* _style);
	~OrdersPanel();
private:
	void createBody() override;
	std::vector<HudElement*>* buttons;
};


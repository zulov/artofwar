#pragma once
#include "../AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>
#include <vector>
#include "hud/HudElement.h"
#include "database/db_strcut.h"
#include "control/SelectedInfo.h"

class MenuPanel :public AbstractWindowPanel
{
public:
	MenuPanel(Urho3D::XMLFile* _style);
	~MenuPanel();
	void removeInfo();
	static Urho3D::String stringFrom(db_unit* dbUnit, std::vector<db_cost*>* costs);
	static Urho3D::String stringFrom(db_building* dbBuilding, std::vector<db_cost*>* costs);
	void setInfo(HudElement* hudElement);
	void updateSelected(SelectedInfo* selectedInfo);

private:
	void createBody() override;
	Urho3D::Text* text;
	Urho3D::Text* text2;
};


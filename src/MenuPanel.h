#pragma once
#include "AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>
#include <array>
#include <vector>
#include <Urho3D/UI/DropDownList.h>
#include "HudElement.h"
#include <Urho3D/UI/Text.h>
#include "db_strcut.h"
#include "SelectedInfo.h"

class MenuPanel :public AbstractWindowPanel
{
public:
	MenuPanel(Urho3D::XMLFile* _style);
	~MenuPanel();
	void removeInfo();
	Urho3D::String stringFrom(db_unit* dbUnit, std::vector<db_unit_cost*>* costs);
	Urho3D::String stringFrom(db_building* dbBuilding, std::vector<db_building_cost*>* costs);
	void setInfo(HudElement* hudElement);
	void updateSelected(SelectedInfo* selectedInfo);

private:
	void createBody() override;
	Urho3D::Text* text;
};


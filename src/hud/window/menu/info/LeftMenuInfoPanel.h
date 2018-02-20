#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>
#include "control/SelectedInfo.h"
#include "hud/HudElement.h"
#include "database/db_strcut.h"

class LeftMenuInfoPanel : public AbstractWindowPanel
{
public:
	LeftMenuInfoPanel(Urho3D::XMLFile* _style);
	~LeftMenuInfoPanel();
	void updateSelected(SelectedInfo* selectedInfo);
	void setInfo(HudElement* hudElement);
	Urho3D::String stringFrom(db_unit* dbUnit, std::vector<db_cost*>* costs);
	Urho3D::String stringFrom(db_building* dbBuilding, std::vector<db_cost*>* costs);
private:
	void createBody() override;

	Urho3D::Text* text;
	Urho3D::Text* text2;
};

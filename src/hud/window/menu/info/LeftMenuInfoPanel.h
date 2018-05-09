#pragma once
#include "control/SelectedInfo.h"
#include "hud/HudData.h"
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>

struct db_cost;

class LeftMenuInfoPanel : public AbstractWindowPanel
{
public:
	LeftMenuInfoPanel(Urho3D::XMLFile* _style);
	~LeftMenuInfoPanel();
	void updateSelected(SelectedInfo* selectedInfo);
	void setInfo(HudData* hudData);
	Urho3D::String stringFrom(String &name, std::vector<db_cost*>* costs);
private:
	void createBody() override;

	Urho3D::Text* text;
	Urho3D::Text* text2;
};

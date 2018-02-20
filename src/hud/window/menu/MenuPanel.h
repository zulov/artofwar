#pragma once
#include "../AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>
#include <vector>
#include "hud/HudElement.h"
#include "database/db_strcut.h"
#include "control/SelectedInfo.h"
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/Button.h>

#define LEFT_MENU_ROWS_NUMBER 4
#define LEFT_MENU_CHECKS_NUMBER 3
#define LEFT_MENU_BUTTON_PER_ROW 4

class MenuPanel : public AbstractWindowPanel
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
	UIElement* rows[LEFT_MENU_ROWS_NUMBER];
	Button* buttons[LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1)];
	Button* nextButton;
	Urho3D::CheckBox* checks[LEFT_MENU_CHECKS_NUMBER];
	UIElement* mock;
};

#pragma once
#include "../AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>
#include <vector>
#include "hud/HudData.h"
#include "database/db_strcut.h"
#include "control/SelectedInfo.h"
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/Button.h>
#include "LeftMenuModes.h"
#include "hud/MySprite.h"
#include <unordered_set>

#define LEFT_MENU_ROWS_NUMBER 4
#define LEFT_MENU_CHECKS_NUMBER 3
#define LEFT_MENU_BUTTON_PER_ROW 4

class LeftMenuInfoPanel;

class MenuPanel : public AbstractWindowPanel
{
public:
	MenuPanel(Urho3D::XMLFile* _style);
	~MenuPanel();
	void removeInfo();
	void refresh(LeftMenuMode _mode, SelectedInfo* selectedInfo);

	void setInfo(HudData* hudElement);
	void updateSelected(SelectedInfo* selectedInfo);
	void setVisible(bool enable) override;
	std::vector<HudData*>& getButtons();
private:
	void createBody() override;
	void setChecks(int val);
	void resetChecks();
	void ChengeModeButton(StringHash eventType, VariantMap& eventData);
	void basicBuilding();
	std::unordered_set<int> getUnitInBuilding(std::vector<SelectedInfoType*>& infoTypes);

	void basicUnit(SelectedInfo* selectedInfo);
	void levelUnit(SelectedInfo* selectedInfo);
	std::unordered_set<int>  getOrderForUnit(std::vector<SelectedInfoType*>& infoTypes);

	void basicOrder(SelectedInfo* selectedInfo);
	
	void resetButtons(int from);
	void updateButtons(SelectedInfo* selectedInfo);

	static Urho3D::String stringFrom(db_unit* dbUnit, std::vector<db_cost*>* costs);
	static Urho3D::String stringFrom(db_building* dbBuilding, std::vector<db_cost*>* costs);

	UIElement* rows[LEFT_MENU_ROWS_NUMBER];
	Button* buttons[LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1)];
	std::vector<HudData*> hudElements;
	MySprite* sprites[LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1)];
	Button* nextButton;
	Urho3D::CheckBox* checks[LEFT_MENU_CHECKS_NUMBER];
	UIElement* mock;
	LeftMenuInfoPanel* infoPanel;
	LeftMenuMode mode = LeftMenuMode::UNIT;
	LeftMenuSubMode subMode = LeftMenuSubMode::BASIC;
	int page = 0;

	SelectedInfo* lastSelectedInfo = nullptr;
};

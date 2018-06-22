#pragma once
#include "../AbstractWindowPanel.h"
#include "LeftMenuModes.h"
#include "control/SelectedInfo.h"
#include "hud/HudData.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <unordered_set>
#include <vector>


#define LEFT_MENU_ROWS_NUMBER 4
#define LEFT_MENU_CHECKS_NUMBER 3
#define LEFT_MENU_BUTTON_PER_ROW 4

class LeftMenuInfoPanel;
class MySprite;

class MenuPanel : public AbstractWindowPanel
{
public:
	MenuPanel(Urho3D::XMLFile* _style);
	~MenuPanel();
	void removeInfo();
	void refresh(LeftMenuMode _mode, SelectedInfo* selectedInfo);

	void setInfo(HudData* hudData);
	void updateSelected(SelectedInfo* selectedInfo);
	void setVisible(bool enable) override;
	std::vector<HudData*>& getButtons();
private:
	void createBody() override;
	void setChecks(int val);

	void ChengeModeButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void setTexture(int k, Urho3D::String textureName);

	void basicUnit(SelectedInfo* selectedInfo);
	void levelUnit(SelectedInfo* selectedInfo);
	void upgradeUnit(SelectedInfo* selectedInfo);
	
	void basicBuilding();
	void levelBuilding();
	
	void basicOrder(SelectedInfo* selectedInfo);
	void formationOrder();


	void unitMenu(SelectedInfo* selectedInfo);
	void buildingMenu();
	void orderMenu(SelectedInfo* selectedInfo);
	
	void resetButtons(int from);
	void updateButtons(SelectedInfo* selectedInfo);

	std::unordered_set<int> getUnitInBuilding(std::vector<SelectedInfoType*>& infoTypes);
	std::unordered_set<int> getUpgradePathInBuilding(std::vector<SelectedInfoType*>& infoTypes);
	std::unordered_set<int>  getOrderForUnit(std::vector<SelectedInfoType*>& infoTypes);

	Urho3D::UIElement* rows[LEFT_MENU_ROWS_NUMBER];
	Urho3D::Button* buttons[LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1)];
	std::vector<HudData*> hudElements;
	MySprite* sprites[LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1)];
	Urho3D::Button* nextButton;
	Urho3D::CheckBox* checks[LEFT_MENU_CHECKS_NUMBER];
	Urho3D::UIElement* mock;
	LeftMenuInfoPanel* infoPanel;
	LeftMenuMode mode = LeftMenuMode::UNIT;
	LeftMenuSubMode subMode = LeftMenuSubMode::BASIC;
	int page = 0;

	SelectedInfo* lastSelectedInfo = nullptr;
};

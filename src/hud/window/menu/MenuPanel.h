#pragma once
#include "../AbstractWindowPanel.h"
#include "LeftMenuModes.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <unordered_set>
#include <vector>


#define LEFT_MENU_ROWS_NUMBER 4
#define LEFT_MENU_CHECKS_NUMBER 3
#define LEFT_MENU_BUTTON_PER_ROW 4

enum class ActionType : char;
enum class LeftMenuMode : char;
class SelectedInfo;
class HudData;
class LeftMenuInfoPanel;
class MySprite;
class SelectedInfoType;

class MenuPanel : public AbstractWindowPanel
{
public:
	explicit MenuPanel(Urho3D::XMLFile* _style);
	~MenuPanel();
	void removeHoverInfo() const;
	void refresh(LeftMenuMode _mode, SelectedInfo* selectedInfo);

	void setHoverInfo(HudData* hudData) const;
	void updateSelected(SelectedInfo* selectedInfo);
	void setVisible(bool enable) override;
	std::vector<HudData*>& getButtons();
	void clearSelected();
private:
	void setCheckVisibility(std::initializer_list<bool> active);
	void updateMode(LeftMenuMode mode);
	void createBody() override;
	void setChecks(int val);

	void ChengeModeButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void setNextElement(int& k, Urho3D::String texture, int id, ActionType menuAction, Urho3D::String text);

	void basicUnit(SelectedInfo* selectedInfo);
	void levelUnit(SelectedInfo* selectedInfo);
	void upgradeUnit(SelectedInfo* selectedInfo);

	void basicBuilding();
	void levelBuilding();

	void basicOrder(SelectedInfo* selectedInfo);
	void formationOrder();


	void buildingMenu();

	void unitMenu(SelectedInfo* selectedInfo);
	void orderMenu(SelectedInfo* selectedInfo);
	void basicResource(SelectedInfo* selectedInfo);
	void resourceMenu(SelectedInfo* selectedInfo);
	void updateButtons(SelectedInfo* selectedInfo);

	void resetButtons(int from);

	std::unordered_set<int> getUnitInBuilding(std::vector<SelectedInfoType*>& infoTypes);
	std::unordered_set<int> getUpgradePathInBuilding(std::vector<SelectedInfoType*>& infoTypes);
	std::unordered_set<int> getOrderForUnit(std::vector<SelectedInfoType*>& infoTypes);
	static void removefromCommon(std::unordered_set<int>& common, std::unordered_set<int>& possibleUntis);

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

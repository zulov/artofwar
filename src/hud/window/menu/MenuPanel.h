#pragma once
#include "../AbstractWindowPanel.h"
#include "LeftMenuModes.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <vector>

#include "database/db_strcut.h"


#define LEFT_MENU_ROWS_NUMBER 4
#define LEFT_MENU_CHECKS_NUMBER 3
#define LEFT_MENU_BUTTON_PER_ROW 4

enum class ActionType : char;
enum class LeftMenuMode : char;
struct db_unit;
class SelectedInfo;
class HudData;
class LeftMenuInfoPanel;
class MySprite;
class SelectedInfoType;

class MenuPanel : public AbstractWindowPanel {
public:
	explicit MenuPanel(Urho3D::XMLFile* _style);
	~MenuPanel();
	void removeHoverInfo() const;
	void refresh(LeftMenuMode _mode, SelectedInfo* selectedInfo);

	void setHoverInfo(HudData* hudData) const;
	void updateSelected(SelectedInfo* selectedInfo) const;
	void setVisible(bool enable) override;
	std::vector<HudData*>& getButtons();
	void clearSelected() const;
private:
	void setCheckVisibility(std::initializer_list<bool> active);
	void updateMode(LeftMenuMode mode);
	void createBody() override;
	void setChecks(int val);

	void ChengeModeButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void setNext(int& k, const Urho3D::String& texture, int id, ActionType menuAction, Urho3D::String text);

	void basicUnit(SelectedInfo* selectedInfo);
	void levelUnit(SelectedInfo* selectedInfo);

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

	void resetRestButtons(int from);

	std::vector<short> getUnitInBuilding(SelectedInfo* selectedInfo);
	std::vector<short> getOrderForUnit(SelectedInfo* selectedInfo);

	Urho3D::UIElement* rows[LEFT_MENU_ROWS_NUMBER];
	Urho3D::Button* buttons[LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1)];
	std::vector<HudData*> hudElements;
	MySprite* sprites[LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1)];
	Urho3D::Button* nextButton;
	Urho3D::CheckBox* checks[LEFT_MENU_CHECKS_NUMBER];
	Urho3D::UIElement* mock;
	LeftMenuInfoPanel* infoPanel;
	SelectedInfo* lastSelectedInfo = nullptr;

	char page = 0;
	LeftMenuMode mode = LeftMenuMode::UNIT;
	LeftMenuSubMode subMode = LeftMenuSubMode::BASIC;

};

#pragma once
#include <vector>
#include "LeftMenuModes.h"
#include "hud/window/EventPanel.h"


namespace Urho3D {
	class Button;
	class CheckBox;
	class XMLFile;
	class UIElement;
}

constexpr char LEFT_MENU_ROWS_NUMBER = 4;
constexpr char LEFT_MENU_CHECKS_NUMBER = 3;
constexpr char LEFT_MENU_BUTTON_PER_ROW = 4;

enum class ActionType : char;
enum class LeftMenuMode : char;
class SelectedInfo;
class HudData;
class LeftMenuInfoPanel;
class MySprite;

class MenuPanel : public EventPanel {
public:
	explicit MenuPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style);
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

	void ChangeModeButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
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

	std::vector<unsigned char> getUnitInBuilding(SelectedInfo* selectedInfo) const;
	std::vector<unsigned char> getOrderForUnit(SelectedInfo* selectedInfo) const;

	Urho3D::UIElement* rows[LEFT_MENU_ROWS_NUMBER];
	Urho3D::Button* buttons[LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1)];
	std::vector<HudData*> hudElements;
	MySprite* sprites[LEFT_MENU_BUTTON_PER_ROW * (LEFT_MENU_ROWS_NUMBER - 1)];

	Urho3D::CheckBox* checks[LEFT_MENU_CHECKS_NUMBER];

	LeftMenuInfoPanel* infoPanel;
	SelectedInfo* lastSelectedInfo = nullptr;

	char page = 0;
	LeftMenuMode mode = LeftMenuMode::UNIT;
	LeftMenuSubMode subMode = LeftMenuSubMode::BASIC;

};

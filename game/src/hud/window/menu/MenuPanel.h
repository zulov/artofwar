#pragma once
#include <vector>
#include "LeftMenuModes.h"
#include "database/db_basic_struct.h"
#include "hud/window/EventPanel.h"


namespace Urho3D {
	class Button;
	class CheckBox;
	class XMLFile;
	class UIElement;
}

constexpr char ROWS_NUMBER = 4;
constexpr char CHECKS_NUMBER = 3;
constexpr char BUTTONS_PER_ROW = 4;
constexpr char BUTTONS_NUMBER = BUTTONS_PER_ROW * (ROWS_NUMBER - 1);

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
	void setChecks(char val);

	void ChangeModeButton(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void NextPage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	void setNext(int& k, const Urho3D::String& texture, short id, ActionType menuAction, Urho3D::String text = "") const;

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

	void resetRestButtons(int from) const;

	std::vector<unsigned char> getUnitInBuilding(SelectedInfo* selectedInfo) const;
	std::vector<unsigned char> getOrderForUnit(SelectedInfo* selectedInfo) const;
	template <typename T>
	void setIcons(const std::vector<T*>& icons, Urho3D::String path, ActionType type);
	void setOrderIcons(const std::vector<char>& ids);

	Urho3D::Button* buttons[BUTTONS_NUMBER];
	std::vector<HudData*> hudElements;
	MySprite* sprites[BUTTONS_NUMBER];

	Urho3D::CheckBox* checks[CHECKS_NUMBER];

	LeftMenuInfoPanel* infoPanel;
	SelectedInfo* lastSelectedInfo = nullptr;

	char page = 0;
	char maxPage = 1;
	LeftMenuMode mode = LeftMenuMode::UNIT;
	LeftMenuSubMode subMode = LeftMenuSubMode::BASIC;

};

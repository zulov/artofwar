#pragma once
#include "hud/window/SimplePanel.h"

namespace Urho3D {
	class Text;
	class XMLFile;
}

class SelectedInfo;
class HudData;
struct db_building;
struct db_building_level;
struct db_unit_level;

class LeftMenuInfoPanel : public SimplePanel {
public:
	explicit LeftMenuInfoPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style);
	~LeftMenuInfoPanel() = default;
	void updateSelected(SelectedInfo* selectedInfo);
	void setHoverInfo(HudData* hudData);
	void setCategoryMessage(const Urho3D::String& message);
	void clearCategoryMessage();
	void removeHoverInfo();
	void clearSelected();
private:
	void appendStat(Urho3D::String& message, const char* labelKey, const Urho3D::String& value);
	void appendStat(Urho3D::String& message, const char* labelKey, float value);
	template <typename T>
	void appendStatIfPositive(Urho3D::String& message, const char* labelKey, T value);
	void appendUnitStats(Urho3D::String& message, const db_unit_level* level, bool upgrade);
	void appendBuildingStats(Urho3D::String& message, const db_building* building, const db_building_level* level,
	                         bool upgrade);
	Urho3D::String createMessage(HudData* hudData);
	void createBody() override;

	Urho3D::Text* text;
	Urho3D::String categoryMessage;
	bool hoverIsOn = false;
	bool selectedIsOn = false;
};

#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>

class SelectedInfo;
class HudData;
struct db_cost;

class LeftMenuInfoPanel : public AbstractWindowPanel
{
public:
	explicit LeftMenuInfoPanel(Urho3D::XMLFile* _style);
	~LeftMenuInfoPanel() = default;
	void updateSelected(SelectedInfo* selectedInfo);
	void setHoverInfo(HudData* hudData);
	void removeHoverInfo();
	void clearSelected();
private:
	Urho3D::String createMessage(HudData* hudData);
	void createBody() override;

	Urho3D::Text* text;
	bool hoverIsOn = false;
	bool selectedIsOn = false;
};

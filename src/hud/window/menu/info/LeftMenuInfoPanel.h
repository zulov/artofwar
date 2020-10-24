#pragma once
#include "hud/window/SimplePanel.h"

namespace Urho3D {
	class Text;
	class XMLFile;
}

class SelectedInfo;
class HudData;

class LeftMenuInfoPanel : public SimplePanel {
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

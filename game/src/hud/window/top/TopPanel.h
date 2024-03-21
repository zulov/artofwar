#pragma once

#include "../SimplePanel.h"
#include "utils/defines.h"


namespace Urho3D { class XMLFile; }
class TopInfoPanel;
class Player;
class TopHudElement;
struct FrameInfo;

class TopPanel : public SimplePanel {
public:
	explicit TopPanel(Urho3D::UIElement* root, Urho3D::XMLFile* style);
	~TopPanel() override;
	void update(Player * player, FrameInfo* frameInfo) const;
	void setVisible(bool enable) override;
private:

	void createBody() override;

	TopHudElement* elements[RESOURCES_SIZE];
	TopHudElement* units;
	TopHudElement* workers;
	TopHudElement* name;
	TopHudElement* time;
	
	TopInfoPanel* infoPanel;
};

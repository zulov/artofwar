#pragma once

#include "../SimplePanel.h"
#include "utils/defines.h"

namespace Urho3D { class XMLFile; }
class TopInfoPanel;
class Player;
class TopHudElement;

class TopPanel : public SimplePanel {
public:
	explicit TopPanel(Urho3D::XMLFile* style);
	~TopPanel();
	void update(Player * player) const;
	void setVisible(bool enable) override;
private:

	void createBody() override;
	TopHudElement* elements[RESOURCES_SIZE];
	TopHudElement* units;
	TopHudElement* workers;
	TopHudElement* name;
	
	TopInfoPanel* infoPanel;
};

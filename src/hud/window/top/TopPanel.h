#pragma once

#include "../AbstractWindowPanel.h"

namespace Urho3D { class XMLFile; }
class TopInfoPanel;
class Player;
class TopHudElement;

class TopPanel : public AbstractWindowPanel {
public:
	explicit TopPanel(Urho3D::XMLFile* _style);
	~TopPanel();
	void update(Player * player) const;
	void setVisible(bool enable) override;
private:

	void createBody() override;
	TopHudElement** elements;
	TopHudElement* units;
	TopHudElement* workers;
	TopHudElement* name;
	
	TopInfoPanel* infoPanel;
};

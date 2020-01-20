#pragma once

#include "../AbstractWindowPanel.h"
#include "info/TopInfoPanel.h"

class Possession;
class Resources;
class TopHudElement;

class TopPanel : public AbstractWindowPanel {
public:
	explicit TopPanel(Urho3D::XMLFile* _style);
	~TopPanel();
	void update(Possession& possession) const;
	void update(Resources& resources) const;
	void setVisible(bool enable) override;
private:

	void createBody() override;
	TopHudElement** elements;
	TopInfoPanel* infoPanel;
	char size;
};

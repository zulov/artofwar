#pragma once

#include "../AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>
#include "info/TopInfoPanel.h"

class Resources;
class TopHudElement;

class TopPanel : public AbstractWindowPanel
{
public:
	explicit TopPanel(Urho3D::XMLFile* _style);
	~TopPanel();
	void update(int value) const;
	void update(Resources& resources) const;
	void setVisible(bool enable) override;
private:

	void createBody() override;
	TopHudElement** elements;
	TopInfoPanel* infoPanel;
};

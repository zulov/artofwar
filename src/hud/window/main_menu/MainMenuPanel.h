#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include "MainMenuDetailsPanel.h"

class MainMenuPanel : public AbstractWindowPanel
{
public:
	MainMenuPanel(Urho3D::XMLFile* _style);
	~MainMenuPanel();
	void setVisible(bool enable) override;
private:
	void createBody() override;
	MainMenuDetailsPanel* detailsPanel;
};

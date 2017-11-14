#pragma once
#include <Urho3D/UI/ProgressBar.h>
#include "hud/window/AbstractWindowPanel.h"

class LoadingPanel : public AbstractWindowPanel
{
public:
	LoadingPanel(Urho3D::XMLFile* _style);
	~LoadingPanel();
	void reset(int _stages);
	void inc();
	void end();
private:
	void createBody() override;
	void update();
	Urho3D::ProgressBar* bar;
	Urho3D::BorderImage* background;
	int stagesNumber = 0;
	float stage = 0;
};

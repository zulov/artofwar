#pragma once
#include <Urho3D/UI/ProgressBar.h>
#include "hud/window/AbstractWindowPanel.h"

class LoadingPanel :public AbstractWindowPanel
{
public:
	LoadingPanel(Urho3D::XMLFile* _style);
	~LoadingPanel();
private:
	void createBody() override;
	void update(float progres);
	Urho3D::ProgressBar* bar;
};


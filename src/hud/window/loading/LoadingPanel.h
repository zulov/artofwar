#pragma once
#include <Urho3D/UI/ProgressBar.h>
#include "hud/window/AbstractWindowPanel.h"

class LoadingPanel : public AbstractWindowPanel
{
public:
	LoadingPanel(Urho3D::XMLFile* _style);
	~LoadingPanel();
	void show();
	void update(float progres);
	void end();
private:
	void createBody() override;
	Urho3D::ProgressBar* bar;
	Urho3D::BorderImage* background;

};

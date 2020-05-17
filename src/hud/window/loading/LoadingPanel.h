#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/ProgressBar.h>

class LoadingPanel : public AbstractWindowPanel {
public:
	explicit LoadingPanel(Urho3D::XMLFile* _style);
	~LoadingPanel();
	void show();
	void update(float progres) const;
	void end();
private:
	void createBody() override;
	Urho3D::ProgressBar* bar;
	Urho3D::BorderImage* background;

};

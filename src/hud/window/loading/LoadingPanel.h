#pragma once
#include "hud/window/AbstractWindowPanel.h"

namespace Urho3D {
	class ProgressBar;
}

class LoadingPanel : public AbstractWindowPanel {
public:
	explicit LoadingPanel(Urho3D::XMLFile* _style);
	~LoadingPanel() = default;
	void show();
	void update(float progres) const;
	void end();
private:
	void createBody() override;
	Urho3D::ProgressBar* bar;
	Urho3D::BorderImage* background;

};

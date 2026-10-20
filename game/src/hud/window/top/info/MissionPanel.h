#pragma once

#include <vector>

#include "hud/window/SimplePanel.h"

namespace Urho3D {
	class Text;
}

class MissionController;

class MissionPanel : public SimplePanel {
public:
	explicit MissionPanel(Urho3D::UIElement* root, Urho3D::XMLFile* style);
	void update();

private:
	void createBody() override;
	Urho3D::Text* getRow(size_t index);

	Urho3D::Text* title{};
	Urho3D::UIElement* rows{};
	std::vector<Urho3D::Text*> rowTexts;
};

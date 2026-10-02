#pragma once
#include <vector>

#include "hud/window/SimplePanel.h"

namespace Urho3D {
	class UIElement;
	class ProgressBar;
	class Text;
}

class Player;
class WorldAgeController;

class TopInfoPanel : public SimplePanel {
public:
	explicit TopInfoPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style);
	~TopInfoPanel() = default;

	void update(const WorldAgeController* controller, const std::vector<Player*>& players, unsigned totalTicks) const;
	void hoverOn();
	void hoverOff();

private:
	void createBody() override;

	bool hoverIsOn = false;
	Urho3D::Text* text{};
	Urho3D::UIElement* rows{};
	Urho3D::Text* timeoutText{};
	Urho3D::ProgressBar* timeoutBar{};
};

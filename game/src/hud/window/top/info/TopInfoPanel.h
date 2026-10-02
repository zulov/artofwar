#pragma once
#include <vector>

#include "hud/window/SimplePanel.h"

namespace Urho3D {
	class String;
	class UIElement;
	class ProgressBar;
	class Text;
	class ToolTip;
}

class Player;
class WorldAgeController;
struct WorldAgeProgress;

class TopInfoPanel : public SimplePanel {
public:
	explicit TopInfoPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style);
	~TopInfoPanel() = default;

	void update(const WorldAgeController* controller, const std::vector<Player*>& players, unsigned totalTicks) const;
	void hoverOn();
	void hoverOff();

private:
	void createBody() override;
	Urho3D::String createAgeTooltip(const WorldAgeController* controller,
		const std::vector<WorldAgeProgress>& ageProgress) const;

	bool hoverIsOn = false;
	Urho3D::Text* text{};
	Urho3D::ToolTip* toolTip{};
	Urho3D::Text* tooltipText{};
	Urho3D::UIElement* rows{};
	Urho3D::Text* timeoutText{};
	Urho3D::ProgressBar* timeoutBar{};
};

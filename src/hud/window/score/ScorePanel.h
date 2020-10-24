#pragma once
#include <vector>

#include "hud/window/SimplePanel.h"

namespace Urho3D {
	class Text;
}

class Player;

class ScorePanel : public SimplePanel {
public:
	explicit ScorePanel(Urho3D::XMLFile* _style);
	~ScorePanel() = default;
	void update(const std::vector<Player*>& players) const;
private:
	void createBody() override;
	Urho3D::Text* text;
};

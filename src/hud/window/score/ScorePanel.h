#pragma once
#include "hud/window/AbstractWindowPanel.h"


namespace Urho3D {
	class Text;
}

class Player;

class ScorePanel : public AbstractWindowPanel {
public:
	explicit ScorePanel(Urho3D::XMLFile* _style);
	~ScorePanel() = default;
	void update(const std::vector<Player*>& players) const;
private:
	void createBody() override;
	Urho3D::Text* text;
};

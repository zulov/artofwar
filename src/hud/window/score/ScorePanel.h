#pragma once
#include "hud/window/AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>
#include "player/Player.h"

class ScorePanel: public AbstractWindowPanel
{
public:
	explicit ScorePanel(Urho3D::XMLFile* _style);
	~ScorePanel();
	void update(const std::vector<Player*>& players);
private:
	void createBody() override;
	Urho3D::Text* text;
};

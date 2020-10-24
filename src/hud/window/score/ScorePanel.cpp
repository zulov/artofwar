#include "ScorePanel.h"
#include <Urho3D/UI/Window.h>
#include "hud/UiUtils.h"
#include "player/Player.h"


ScorePanel::ScorePanel(Urho3D::XMLFile* _style)
	: SimplePanel(_style, "ScorePanel", {
		                      GameState::RUNNING, GameState::PAUSE
	                      }) {}

void ScorePanel::update(const std::vector<Player*>& players) const {
	Urho3D::String s = "";
	for (auto player : players) {
		s.Append(player->getName())
		 .Append(" - ")
		 .Append(Urho3D::String(player->getScore()))
		 .Append("\n");
	}
	text->SetText(s);
}

void ScorePanel::createBody() {
	text = addChildText(window, "MyText", style);
}

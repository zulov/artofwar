#include "ScorePanel.h"
#include "hud/UiUtils.h"


ScorePanel::ScorePanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "ScorePanel", {
	                                                                      GameState::RUNNING, GameState::PAUSE
                                                                      }) {
}

ScorePanel::~ScorePanel() = default;

void ScorePanel::update(const std::vector<Player*>& players) {
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
	Urho3D::String a = "test";
	text = addChildText(window, "MyText", a, style);
}
#include "LoadingPanel.h"
#include "GameState.h"


LoadingPanel::LoadingPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "LoadingWindow";
	
	visibleAt[static_cast<char>(GameState::LOADING)] = true;
	visibleAt[static_cast<char>(GameState::NEW_GAME)] = true;
}


LoadingPanel::~LoadingPanel() = default;

void LoadingPanel::show() {
	setVisible(true);

	update(0);
}

void LoadingPanel::end() {
	setVisible(false);
}

void LoadingPanel::createBody() {
	background = window->CreateChild<Urho3D::BorderImage>();
	background->SetStyle("Background", style);
	background->SetVisible(true);

	bar = background->CreateChild<Urho3D::ProgressBar>();
	bar->SetStyle("LargeProgressBar", style);
	bar->SetRange(1);
	bar->SetValue(0);
	bar->SetVisible(true);
}

void LoadingPanel::update(float progres) {
	bar->SetValue(progres);
}

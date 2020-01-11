#include "LoadingPanel.h"
#include "GameState.h"
#include "hud/UiUtils.h"


LoadingPanel::LoadingPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "LoadingWindow",
                                                                          {GameState::LOADING, GameState::NEW_GAME}) {
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
	background = createElement<Urho3D::BorderImage>(window, style, "Background");
	background->SetVisible(true);

	bar = createElement<Urho3D::ProgressBar>(background, style, "LargeProgressBar");
	bar->SetRange(1);
	bar->SetValue(0);
	bar->SetVisible(true);
}

void LoadingPanel::update(float progres) const {
	bar->SetValue(progres);
}

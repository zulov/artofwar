#include "LoadingPanel.h"

#include <Urho3D/UI/ProgressBar.h>
#include <Urho3D/UI/Window.h>
#include "hud/UiUtils.h"

LoadingPanel::LoadingPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style) : SimplePanel(root, _style, "LoadingWindow",
                                                                          {GameState::LOADING, GameState::NEW_GAME}) {}

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
	bar->SetRange(1.f);
	bar->SetValue(0.f);
	bar->SetVisible(true);
}

void LoadingPanel::update(float progres) const {
	bar->SetValue(progres);
}

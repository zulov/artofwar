#include "LoadingPanel.h"
#include "Game.h"
#include <Urho3D/UI/UI.h>


LoadingPanel::LoadingPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "LoadingWindow";
}


LoadingPanel::~LoadingPanel() {
}

void LoadingPanel::reset(int _stages) {
	setVisible(true);
	stagesNumber = _stages;
	stage = 0;
	update();
}

void LoadingPanel::inc() {
	++stage;
	update();
}

void LoadingPanel::end() {
	setVisible(false);
}

void LoadingPanel::createBody() {

	background = window->CreateChild<Urho3D::BorderImage>();
	background->SetStyle("Background", style);
	background->SetVisible(true);

	bar = window->CreateChild<Urho3D::ProgressBar>();
	bar->SetStyle("LargeProgressBar", style);
	bar->SetRange(1);
	bar->SetValue(0);
	bar->SetVisible(true);

}

void LoadingPanel::update() {
	float val = stage / stagesNumber;
	bar->SetValue(val);
}

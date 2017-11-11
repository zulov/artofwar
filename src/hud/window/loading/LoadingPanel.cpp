#include "LoadingPanel.h"
#include "Game.h"
#include <Urho3D/UI/UI.h>


LoadingPanel::LoadingPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {

}


LoadingPanel::~LoadingPanel() {
}

void LoadingPanel::createBody() {

	bar = Game::get()->getUI()->GetRoot()->CreateChild<Urho3D::ProgressBar>();
	bar->SetStyle("LargeProgressBar", style);
	bar->SetRange(100);
	bar->SetValue(0);
	bar->SetVisible(true);

}

void LoadingPanel::update(float progres) {
	bar->SetValue(progres);
}

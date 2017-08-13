#include "DebugPanel.h"


DebugPanel::DebugPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
}


DebugPanel::~DebugPanel() {
}

void DebugPanel::setText(const Urho3D::String& msg) {
	fpsText->SetText(msg);
}

void DebugPanel::createBody() {
	fpsText = window->CreateChild<Urho3D::Text>();
	fpsText->SetStyle("MyText", style);
}

#include "DebugPanel.h"


DebugPanel::DebugPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style) {
	styleName = "MyDebugHudWindow";
	msg = new  Urho3D::String();
}


DebugPanel::~DebugPanel() {
	delete msg;
}

void DebugPanel::setText(double getLastFps, double getAverageFps, long getLoops, Urho3D::String* string) {
	msg->Clear();
	msg->Append("FPS: ").Append(Urho3D::String(getLastFps))
		.Append("\navg FPS: ").Append(Urho3D::String(getAverageFps))
		.Append("\nLoops: ").Append(Urho3D::String(getLoops))
		.Append("\nCamera: \n\t").Append(*string);

	fpsText->SetText(*msg);
}

void DebugPanel::createBody() {
	fpsText = window->CreateChild<Urho3D::Text>();
	fpsText->SetStyle("MyText", style);
}

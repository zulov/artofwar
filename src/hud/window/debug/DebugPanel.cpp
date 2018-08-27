#include "DebugPanel.h"
#include "GameState.h"


DebugPanel::DebugPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "MyDebugHudWindow") {
	visibleAt[static_cast<char>(GameState::RUNNING)] = true;
	visibleAt[static_cast<char>(GameState::PAUSE)] = true;
}


DebugPanel::~DebugPanel() = default;

void DebugPanel::setText(float getLastFps, float getAverageFps, int getLoops, Urho3D::String* string) {
	msg.Clear();
	msg.Append("FPS: ").Append(Urho3D::String(getLastFps))
	   .Append("\navg FPS: ").Append(Urho3D::String(getAverageFps))
	   .Append("\nLoops: ").Append(Urho3D::String(getLoops))
	   .Append("\nCamera: \n\t").Append(*string);

	fpsText->SetText(msg);
}

void DebugPanel::createBody() {
	fpsText = window->CreateChild<Urho3D::Text>();
	fpsText->SetStyle("MyText", style);
}

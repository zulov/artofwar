#include "DebugPanel.h"
#include "GameState.h"
#include "hud/UiUtils.h"


DebugPanel::DebugPanel(Urho3D::XMLFile* _style) : AbstractWindowPanel(_style, "MyDebugHudWindow",
                                                                      {GameState::RUNNING, GameState::PAUSE}) {
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
	fpsText = addChildText(window, "MyText", "", style);
}

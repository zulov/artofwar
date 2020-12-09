#include "DebugPanel.h"
#include "hud/UiUtils.h"
#include <Urho3D/UI/Window.h>

DebugPanel::DebugPanel(Urho3D::XMLFile* _style) : SimplePanel(_style, "MyDebugHudWindow",
                                                                      {GameState::RUNNING, GameState::PAUSE}) {}


void DebugPanel::setText(float getLastFps, float getAverageFps, int getLoops, float avgLow, float avgMiddle,
                         float avgHighest, const Urho3D::String& camInfo) {
	msg.Clear();
	msg.Append("FPS: ").Append(Urho3D::String(getLastFps))
	   .Append("\navg FPS: ").Append(Urho3D::String(getAverageFps))
	   .Append("\nPercentiles ")
	   .Append(Urho3D::String(avgLow)).Append("|")
	   .Append(Urho3D::String(avgMiddle)).Append("|")
	   .Append(Urho3D::String(avgHighest)).Append("|")
	   .Append("\nLoops: ").Append(Urho3D::String(getLoops))
	   .Append("\nCamera: \n\t").Append(camInfo);

	fpsText->SetText(msg);
}

void DebugPanel::createBody() {
	fpsText = addChildText(window, "MyText", style);
}

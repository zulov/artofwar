#include "DebugPanel.h"
#include "hud/UiUtils.h"
#include <Urho3D/UI/Window.h>

DebugPanel::DebugPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style) : SimplePanel(root, _style, "MyDebugHudWindow",
                                                              {GameState::RUNNING, GameState::PAUSE}) {
}


void DebugPanel::setText(unsigned int seconds, float avgLow, float avgMiddle,
                         float avgHighest, const Urho3D::String& camInfo) {
	msg.Clear();
	msg.Append("FPS: ")
	   .Append(Urho3D::String(avgLow)).Append("|")
	   .Append(Urho3D::String(avgMiddle)).Append("|")
	   .Append(Urho3D::String(avgHighest)).Append("|")
	   .Append("\nCamera: \n\t").Append(camInfo);

	fpsText->SetText(msg);
}

void DebugPanel::createBody() {
	fpsText = addChildText(window, "MyText", style);
}

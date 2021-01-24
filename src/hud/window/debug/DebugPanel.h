#pragma once
#include "../SimplePanel.h"


namespace Urho3D {
	class Text;
	class XMLFile;
}

class DebugPanel : public SimplePanel {
public:
	explicit DebugPanel(Urho3D::XMLFile* _style);
	~DebugPanel() = default;
	void setText(float getLastFps, float getAverageFps, unsigned int seconds, float avgLow, float avgMiddle,
	             float avgHighest, const Urho3D::String& camInfo);

private:
	void createBody() override;
	Urho3D::Text* fpsText;
	Urho3D::String msg = "";
};

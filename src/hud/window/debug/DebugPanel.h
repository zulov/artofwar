#pragma once
#include "../AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>

namespace Urho3D {
	class XMLFile;
}

class DebugPanel : public AbstractWindowPanel
{
public:
	DebugPanel(Urho3D::XMLFile* _style);
	~DebugPanel();
	void setText(float getLastFps, float getAverageFps, int getLoops, Urho3D::String* string);

private:
	void createBody() override;
	Urho3D::Text* fpsText;
	Urho3D::String msg = "";
};

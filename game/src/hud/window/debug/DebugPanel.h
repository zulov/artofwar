#pragma once
#include "../SimplePanel.h"
#include "simulation/SimGlobals.h"

#include <array>


namespace Urho3D {
	class Text;
	class XMLFile;
}

class DebugPanel : public SimplePanel {
public:
	explicit DebugPanel(Urho3D::UIElement* root, Urho3D::XMLFile* _style);
	~DebugPanel() = default;
	void setText(float avgLow, float avgMiddle,
	             float avgHighest, const float* tickAvgs, const Urho3D::String& camInfo);

private:
	void createBody() override;
	Urho3D::Text* summaryText{};
	Urho3D::Text* cameraText{};
	std::array<Urho3D::Text*, FRAMES_IN_PERIOD> tickTexts{};
};

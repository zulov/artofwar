#pragma once
#include "hud/window/main_menu/MainMenuDetailsPanel.h"

namespace Urho3D {
	class Button;
	class ScrollView;
	class ListView;
}

class MainMenuLoadPanel : public MainMenuDetailsPanel {
public:
	MainMenuLoadPanel(Urho3D::XMLFile* _style, const Urho3D::String& _title);
	~MainMenuLoadPanel() = default;
	void HandleLoadClick(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
	Urho3D::Button* getLoadButton() const;
private:
	void createBody() override;
	void action(const Urho3D::String& saveName);
	Urho3D::UIElement* leftMock;
	Urho3D::ListView* list;
	Urho3D::ScrollView* content;

	Urho3D::Button* loadButton;
};

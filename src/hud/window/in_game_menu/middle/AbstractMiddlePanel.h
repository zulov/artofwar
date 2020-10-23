#pragma once
#include "hud/window/AbstractWindowPanel.h"

namespace Urho3D {
	class UIElement;
	class Text;
	class Button;
}

constexpr char MIDDLE_LIST_ROW_NUMBER = 3;

class AbstractMiddlePanel : public AbstractWindowPanel {
public:
	AbstractMiddlePanel(Urho3D::XMLFile* _style, const Urho3D::String& _title);
	virtual ~AbstractMiddlePanel();

	void createBody() override;
	void HandleClose(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	virtual Urho3D::Button* getMainButton();
protected:
	Urho3D::UIElement* body;

private:
	Urho3D::Button* buttonClose;
	Urho3D::Text* title;
	Urho3D::String msg;
};

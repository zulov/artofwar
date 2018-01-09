#pragma once
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>
#include "hud/window/AbstractWindowPanel.h"

#define MIDDLE_LIST_ROW_NUMBER 3

class AbstractMiddlePanel : public AbstractWindowPanel
{
public:
	AbstractMiddlePanel(Urho3D::XMLFile* _style, Urho3D::String _title);
	virtual ~AbstractMiddlePanel();

	void createBody() override;
	void HandleClose(Urho3D::StringHash, Urho3D::VariantMap& eventData);
	virtual Urho3D::Button* getMainButton();
protected:
	Urho3D::UIElement * body;

private:
	Urho3D::Button* buttonClose;
	Urho3D::Text* title;
	Urho3D::String msg;

};

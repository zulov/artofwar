#pragma once
#include "../AbstractWindowPanel.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Text.h>

class AbstractMiddlePanel : public AbstractWindowPanel
{
public:
	AbstractMiddlePanel(Urho3D::XMLFile* _style);
	~AbstractMiddlePanel();

	void createBody() override;
	Urho3D::Button* getCloseButton();
	void Handle(Urho3D::StringHash, Urho3D::VariantMap& eventData);
private:
	Urho3D::Button* buttonClose;
	Urho3D::Text * title;
};

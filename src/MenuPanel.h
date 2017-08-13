#pragma once
#include "AbstractWindowPanel.h"
#include <Urho3D/UI/Text.h>
#include <array>
#include <vector>
#include <Urho3D/UI/DropDownList.h>
#include "HudElement.h"
#include <Urho3D/UI/Text.h>

class MenuPanel :public AbstractWindowPanel
{
public:
	MenuPanel(Urho3D::XMLFile* _style);
	~MenuPanel();
	std::vector<HudElement*>* getLists();
private:
	void createBody() override;
	Urho3D::Text* text;
	std::vector<HudElement*>* lists;
};


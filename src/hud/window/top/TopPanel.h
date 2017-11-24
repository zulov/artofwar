#pragma once

#include "../AbstractWindowPanel.h"
#include "TopHudElement.h"
#include <vector>
#include "player/Resources.h"
#include <Urho3D/UI/DropDownList.h>


class TopPanel :public AbstractWindowPanel
{
public:
	TopPanel(Urho3D::XMLFile* _style);
	~TopPanel();
	std::vector<Button*>* getButtonsSelectedToSubscribe();
	void update(int value);
	void update(Resources* resources);
private:
	
	void createBody() override;
	std::vector<Button*>* buttons;
	TopHudElement** elements;
	DropDownList * menuList;
	Text * unitsNumber;
};


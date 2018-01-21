#pragma once

#include "../AbstractWindowPanel.h"
#include "TopHudElement.h"
#include <vector>
#include "player/Resources.h"
#include <Urho3D/UI/DropDownList.h>


class TopPanel :public AbstractWindowPanel
{
public:
	TopPanel();
	~TopPanel();
	void update(int value);
	void update(Resources& resources);
private:
	
	void createBody() override;
	TopHudElement** elements;
	Text * unitsNumber;
};


#pragma once

#include "../AbstractWindowPanel.h"
#include "TopHudElement.h"
#include "player/Resources.h"


class TopPanel :public AbstractWindowPanel
{
public:
	TopPanel(Urho3D::XMLFile* _style);
	~TopPanel();
	void update(int value) const;
	void update(Resources& resources) const;
private:
	
	void createBody() override;
	TopHudElement** elements;
	Text * unitsNumber;
};


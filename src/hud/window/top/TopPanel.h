#pragma once

#include "../AbstractWindowPanel.h"
#include "TopHudElement.h"

class Resources;

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
	Urho3D::Text * unitsNumber;
};


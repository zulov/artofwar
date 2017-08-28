#pragma once

#include "AbstractWindowPanel.h"
#include "TopHudElement.h"
#include <vector>
#include <Urho3D/Graphics/Texture2D.h>
#include "Resources.h"


class TopPanel :public AbstractWindowPanel
{
public:
	TopPanel(Urho3D::XMLFile* _style);
	~TopPanel();
	std::vector<Button*>* getButtonsSelectedToSubscribe();
	void update(Resources* resources);
private:
	
	void createBody() override;
	std::vector<Button*>* buttons;
	TopHudElement** elements;
};


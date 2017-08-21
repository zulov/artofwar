#pragma once
#include <Urho3D/UI/Button.h>
#include "ObjectEnums.h"
#include "Game.h"
#include "SelectedInfo.h"
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Graphics/Texture2D.h>
#include "SelectedHudElement.h"
#include "AbstractWindowPanel.h"

class SelectedHudPanel :public AbstractWindowPanel
{
public:
	SelectedHudPanel(Urho3D::XMLFile* _style);
	~SelectedHudPanel();	
	void update(SelectedInfo* selectedInfo);
	
	std::vector<Button*>* getButtonsSelectedToSubscribe();
	
private:
	void hide();
	void createBody() override;
	short MAX_ICON_SELECTION = 31;
	short LINES_IN_SELECTION = 4;
	String getIconName(ObjectType index, int i);
	int getSize(ObjectType type);
	std::vector<Button*>* buttons;
	SelectedHudElement** elements;
	UIElement **rows;
};

